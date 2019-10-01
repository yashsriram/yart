#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <unordered_map>
#include <sstream>
#include "light.hpp"

using namespace std;

class Scene {
public:
    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Sphere &);

    const string filename;

    Vector3D eye;
    Vector3D viewDir;
    Vector3D upDir;

    float vFovDeg;
    float imWidth;
    float imHeight;

    Color bgColor;
    vector<Sphere> spheres;
    vector<Ellipsoid> ellipsoids;

    vector<Light> lights;

    Scene(const string &filename) : filename(filename),
                                    eye(Vector3D()), viewDir(Vector3D()), upDir(Vector3D()),
                                    vFovDeg(0), imWidth(0), imHeight(0),
                                    bgColor(Color()) {}

    // Reads the scene description and validates it
    // If everything is valid returns true else returns false and prints and error message
    // If true is returned the scene description is stored in object variables
    bool readAndValidate() {
        // Opening input file
        ifstream input(this->filename.c_str());

        if (input.fail()) {
            // Error handling: if file could not be opened
            cerr << "Input file named \"" << this->filename
                 << "\" could not be opened. Maybe it doesn't exist or has insufficient permissions." << endl;
            return false;
        }

        unordered_map<string, int> keywordsCheck;
        keywordsCheck["eye"] = 0;
        keywordsCheck["viewdir"] = 0;
        keywordsCheck["updir"] = 0;
        keywordsCheck["vfov"] = 0;
        keywordsCheck["imsize"] = 0;
        keywordsCheck["bkgcolor"] = 0;
        keywordsCheck["mtlcolor"] = 0;
        MaterialColor mtlcolor;
        bool mtlcolorExists = false;

        string line;
        while (getline(input, line)) {
            string keyword;
            istringstream iss(line);

            // Keyword validation
            if (!(iss >> keyword)) {
                continue;
            }
            // Comment
            if (keyword == "#") {
                continue;
            } else if (keyword == "eye") {
                if (!this->readEye(iss)) {
                    input.close();
                    return false;
                }
                keywordsCheck[keyword] = 1;
            } else if (keyword == "viewdir") {
                if (!this->readViewDir(iss)) {
                    input.close();
                    return false;
                }
                keywordsCheck[keyword] = 1;
            } else if (keyword == "updir") {
                if (!this->readUpDir(iss)) {
                    input.close();
                    return false;
                }
                keywordsCheck[keyword] = 1;
            } else if (keyword == "vfov") {
                if (!this->readVFov(iss)) {
                    input.close();
                    return false;
                }
                keywordsCheck[keyword] = 1;
            } else if (keyword == "imsize") {
                if (!this->readImageDim(iss)) {
                    input.close();
                    return false;
                }
                keywordsCheck[keyword] = 1;
            } else if (keyword == "bkgcolor") {
                if (!this->readBgColor(iss)) {
                    input.close();
                    return false;
                }
                keywordsCheck[keyword] = 1;
            } else if (keyword == "mtlcolor") {
                if (!this->readMtlColor(iss, mtlcolor)) {
                    input.close();
                    return false;
                }
                keywordsCheck[keyword] = 1;
                mtlcolorExists = true;
            } else if (keyword == "sphere") {
                if (!mtlcolorExists) {
                    cerr << "Syntax error in input file \"" << this->filename
                         << "\". Sphere information found without preceeding mtl color" << endl;
                    return false;
                }
                if (!this->readSphere(iss, mtlcolor)) {
                    input.close();
                    return false;
                }
            } else if (keyword == "ellipsoid") {
                if (!mtlcolorExists) {
                    cerr << "Syntax error in input file \"" << this->filename
                         << "\". Ellipsoid information found without preceeding mtl color" << endl;
                    return false;
                }
                if (!this->readEllipsoid(iss, mtlcolor)) {
                    input.close();
                    return false;
                }
            } else if (keyword == "light") {
                if (!this->readLight(iss)) {
                    input.close();
                    return false;
                }
            } else {
                cerr << "Syntax error in input file \"" << this->filename << "\". Invalid keyword" << endl;
                return false;
            }
        }

        // Parallel view and up vector check
        if (this->upDir.dot(this->viewDir) == 1 || this->upDir.dot(this->viewDir) == -1) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Parallel/Anti-parallel up and view directions! " << endl;
            input.close();
            return false;
        }

        for (const auto &i : keywordsCheck) {
            if (i.second != 1) {
                cerr << "Syntax error in input file \"" << this->filename << "\". Information missing: " << i.first
                     << endl;
                return false;
            }
        }

        input.close();
        return true;
    }

private:
    bool readEye(istringstream &iss) {
        // Value validation
        float x, y, z;
        if (!(iss >> x)) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Eye x-coordinate Information missing"
                 << endl;
            return false;
        }
        if (!(iss >> y)) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Eye y-coordinate Information missing"
                 << endl;
            return false;
        }
        if (!(iss >> z)) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Eye z-coordinate Information missing"
                 << endl;
            return false;
        }

        // Setting scene variable
        this->eye = Vector3D(x, y, z);

        return true;
    }

    bool readViewDir(istringstream &iss) {
        // Value validation
        float x, y, z;
        if (!(iss >> x)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". View Direction x-coordinate Information missing" << endl;
            return false;
        }
        if (!(iss >> y)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". View Direction y-coordinate Information missing" << endl;
            return false;
        }
        if (!(iss >> z)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". View Direction z-coordinate Information missing" << endl;
            return false;
        }
        Vector3D viewDir = Vector3D(x, y, z);
        if (viewDir.abs() - 1 > 1e-6) {
            cerr << "Syntax error in input file \"" << this->filename << "\". View Direction is not unit vector" << endl;
            return false;
        }

        // Setting scene variable
        this->viewDir = viewDir;

        return true;
    }

    bool readUpDir(istringstream &iss) {
        // Value validation
        float x, y, z;
        if (!(iss >> x)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Up Direction x-coordinate Information missing" << endl;
            return false;
        }
        if (!(iss >> y)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Up Direction y-coordinate Information missing" << endl;
            return false;
        }
        if (!(iss >> z)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Up Direction z-coordinate Information missing" << endl;
            return false;
        }
        Vector3D upDir = Vector3D(x, y, z);
        if (upDir.abs() - 1 > 1e-6) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Up Direction is not unit vector" << endl;
            return false;
        }

        // Setting scene variable
        this->upDir = upDir;

        return true;
    }

    bool readVFov(istringstream &iss) {
        // Value validation
        float vFovDeg;
        if (!(iss >> vFovDeg)) {
            cerr << "Syntax error in input file \"" << this->filename << "\". VFov Information missing" << endl;
            return false;
        }
        if (vFovDeg <= 0 || vFovDeg >= 180) {
            cerr << "Syntax error in input file \"" << this->filename << "\". VFov is Invalid" << endl;
            return false;
        }

        // Setting scene variable
        this->vFovDeg = vFovDeg;

        return true;
    }

    bool readImageDim(istringstream &iss) {
        // Value validation
        int imWidth, imHeight;
        if (!(iss >> imWidth)) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Image dimensions width Information missing"
                 << endl;
            return false;
        }
        if (!(iss >> imHeight)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Image dimensions height Information missing" << endl;
            return false;
        }
        if (imWidth <= 0) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Image dimensions width is Invalid" << endl;
            return false;
        }
        if (imHeight <= 0) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Image dimensions width is Invalid" << endl;
            return false;
        }

        // Setting scene variable
        this->imWidth = imWidth;
        this->imHeight = imHeight;

        return true;
    }

    bool readBgColor(istringstream &iss) {
        // Value validation
        float r, g, b;
        if (!(iss >> r)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Background color r-coordinate Information missing" << endl;
            return false;
        }
        if (!(iss >> g)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Background color g-coordinate Information missing" << endl;
            return false;
        }
        if (!(iss >> b)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Background color b-coordinate Information missing" << endl;
            return false;
        }
        if (r < 0 || r > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Background color r is not between 0 and 1"
                 << endl;
            return false;
        }
        if (g < 0 || g > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Background color g is not between 0 and 1"
                 << endl;
            return false;
        }
        if (b < 0 || b > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Background color b is not between 0 and 1"
                 << endl;
            return false;
        }

        // Setting scene variable
        this->bgColor = Color(r, g, b);

        return true;
    }

    bool readMtlColor(istringstream &iss, MaterialColor &color) {
        float dr, dg, db, sr, sg, sb, ka, kd, ks;
        int n;
        if (!(iss >> dr)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color dr missing" << endl;
            return false;
        }
        if (!(iss >> dg)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color dg missing" << endl;
            return false;
        }
        if (!(iss >> db)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color db missing" << endl;
            return false;
        }
        if (!(iss >> sr)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color sr missing" << endl;
            return false;
        }
        if (!(iss >> sg)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color sg missing" << endl;
            return false;
        }
        if (!(iss >> sb)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color sb missing" << endl;
            return false;
        }
        if (!(iss >> ka)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color ka missing" << endl;
            return false;
        }
        if (!(iss >> kd)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color kd missing" << endl;
            return false;
        }
        if (!(iss >> ks)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color ks missing" << endl;
            return false;
        }
        if (!(iss >> n)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Material color n missing" << endl;
            return false;
        }
        if (dr < 0 || dr > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color dr is not between 0 and 1"
                 << endl;
            return false;
        }
        if (dg < 0 || dg > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color dg is not between 0 and 1"
                 << endl;
            return false;
        }
        if (db < 0 || db > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color db is not between 0 and 1"
                 << endl;
            return false;
        }
        if (sr < 0 || sr > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color sr is not between 0 and 1"
                 << endl;
            return false;
        }
        if (sg < 0 || sg > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color sg is not between 0 and 1"
                 << endl;
            return false;
        }
        if (sb < 0 || sb > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color sb is not between 0 and 1"
                 << endl;
            return false;
        }
        if (ka < 0 || ka > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color ka is not between 0 and 1"
                 << endl;
            return false;
        }
        if (kd < 0 || kd > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color kd is not between 0 and 1"
                 << endl;
            return false;
        }
        if (ks < 0 || ks > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color ks is not between 0 and 1"
                 << endl;
            return false;
        }
        if (n < 0) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Material color n is negative" << endl;
            return false;
        }
        // update state variable material color
        color = MaterialColor(Color(dr, dg, db), Color(sr, sg, sb), ka, kd, ks, n);

        return true;
    }

    bool readSphere(istringstream &iss, MaterialColor &color) {
        float x, y, z, rad;
        if (!(iss >> x)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Sphere x-coordinate missing" << endl;
            return false;
        }
        if (!(iss >> y)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Sphere y-coordinate missing" << endl;
            return false;
        }
        if (!(iss >> z)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Sphere z-coordinate missing" << endl;
            return false;
        }
        if (!(iss >> rad)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Sphere radius missing" << endl;
            return false;
        }
        if (rad <= 0) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Sphere radius is non-positive"
                 << endl;
            return false;
        }

        // Setting scene variable
        this->spheres.emplace_back(Vector3D(x, y, z), rad, color);

        return true;
    }

    bool readEllipsoid(istringstream &iss, MaterialColor &color) {
        float x, y, z, rx, ry, rz;
        if (!(iss >> x)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid x-coordinate missing" << endl;
            return false;
        }
        if (!(iss >> y)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid y-coordinate missing" << endl;
            return false;
        }
        if (!(iss >> z)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid z-coordinate missing" << endl;
            return false;
        }
        if (!(iss >> rx)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid radius missing" << endl;
            return false;
        }
        if (!(iss >> ry)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid radius missing" << endl;
            return false;
        }
        if (!(iss >> rz)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid radius missing" << endl;
            return false;
        }
        if (rx <= 0) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid radius is non-positive" << endl;
            return false;
        }
        if (ry <= 0) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid radius is non-positive" << endl;
            return false;
        }
        if (rz <= 0) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Ellipsoid radius is non-positive" << endl;
            return false;
        }

        // Setting scene variable
        this->ellipsoids.emplace_back(Vector3D(x, y, z), rx, ry, rz, color);

        return true;
    }

    bool readLight(istringstream &iss) {
        float r, g, b, x, y, z;
        int w;
        if (!(iss >> x)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Light x missing" << endl;
            return false;
        }
        if (!(iss >> y)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Light y missing" << endl;
            return false;
        }
        if (!(iss >> z)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Light z missing" << endl;
            return false;
        }
        if (!(iss >> w)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Light w missing" << endl;
            return false;
        }
        if (!(iss >> r)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Light r missing" << endl;
            return false;
        }
        if (!(iss >> g)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Light g missing" << endl;
            return false;
        }
        if (!(iss >> b)) {
            cerr << "Syntax error in input file \"" << this->filename
                 << "\". Light b missing" << endl;
            return false;
        }
        if (r < 0 || r > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Light r is not between 0 and 1"
                 << endl;
            return false;
        }
        if (g < 0 || g > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Light g is not between 0 and 1"
                 << endl;
            return false;
        }
        if (b < 0 || b > 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Light b is not between 0 and 1"
                 << endl;
            return false;
        }
        if (w != 0 && w != 1) {
            cerr << "Syntax error in input file \"" << this->filename << "\". Light w is invalid" << endl;
            return false;
        }
        // update state variable lights
        Light light(Vector3D(x, y, z), w, Color(r, g, b));
        this->lights.emplace_back(light);

        return true;
    }
};

// this is to easily print a given object in a well-formatted manner to std for debugging
std::ostream &operator<<(std::ostream &out, const Scene &s) {
    out << "==== Scene ====" << endl;
    out << "Eye:\t" << s.eye << endl;
    out << "V dir:\t" << s.viewDir << endl;
    out << "Up dir:\t" << s.upDir << endl;
    out << "VFOV:\t" << s.vFovDeg << endl;
    out << "(w, h):\t" << s.imWidth << ", " << s.imHeight << endl;
    out << "Bg clr: " << s.bgColor << endl;
    for (const Sphere &sphere: s.spheres) {
        out << sphere << endl;
    }
    for (const Ellipsoid &ellipsoid: s.ellipsoids) {
        out << ellipsoid << endl;
    }
    for (const Light &light: s.lights) {
        out << light << endl;
    }
    return out;
}

#endif
