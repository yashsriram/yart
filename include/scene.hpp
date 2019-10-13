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

        unordered_map<string, int> criticalInputCheck;
        criticalInputCheck["eye"] = 0;
        criticalInputCheck["viewdir"] = 0;
        criticalInputCheck["updir"] = 0;
        criticalInputCheck["vfov"] = 0;
        criticalInputCheck["imsize"] = 0;
        criticalInputCheck["bkgcolor"] = 0;
        MaterialColor mtlColor;
        bool mtlColorExists = false;

        cout << "Parsing file \"" << this->filename << "\"." << endl;
        string line;
        while (getline(input, line)) {
            string keyword;
            istringstream iss(line);
            // Keyword validation
            if (!(iss >> keyword)) {
                continue;
            }
            // Line identifier switching
            if (keyword == "#") {
                continue;
            }
                // Critical input
            else if (keyword == "eye") {
                if (!this->readEye(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "viewdir") {
                if (!this->readViewDir(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "updir") {
                if (!this->readUpDir(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "vfov") {
                if (!this->readVFov(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "imsize") {
                if (!this->readImageSize(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "bkgcolor") {
                if (!this->readBgColor(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            }
                // Non-critical or optional input
            else if (keyword == "mtlcolor") {
                if (!this->readMtlColor(iss, mtlColor)) {
                    input.close();
                    return false;
                }
                mtlColorExists = true;
            } else if (keyword == "sphere") {
                if (!mtlColorExists) {
                    cerr << "Sphere information found without preceding mtl color" << endl;
                    return false;
                }
                if (!this->readSphere(iss, mtlColor)) {
                    input.close();
                    return false;
                }
            } else if (keyword == "ellipsoid") {
                if (!mtlColorExists) {
                    cerr << "Ellipsoid information found without preceding mtl color" << endl;
                    return false;
                }
                if (!this->readEllipsoid(iss, mtlColor)) {
                    input.close();
                    return false;
                }
            } else if (keyword == "light") {
                if (!this->readLight(iss)) {
                    input.close();
                    return false;
                }
            } else {
                cerr << "Invalid keyword found: " << keyword << ". Ignoring it" << endl;
                continue;
            }
        }
        // Parallel view and up vector check
        if (this->upDir.dot(this->viewDir) == 1 || this->upDir.dot(this->viewDir) == -1) {
            cerr << "Parallel/Anti-parallel up and view directions! " << endl;
            input.close();
            return false;
        }
        // All necessary keywords obtained check
        for (const auto &i : criticalInputCheck) {
            if (i.second != 1) {
                cerr << "Critical information missing: " << i.first << endl;
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
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "Eye coordinates incomplete" << endl;
            return false;
        }
        // Setting scene variable
        this->eye = Vector3D(x, y, z);
        return true;
    }

    bool readViewDir(istringstream &iss) {
        // Value validation
        float x, y, z;
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "View Direction coordinates incomplete" << endl;
            return false;
        }
        Vector3D _viewDir = Vector3D(x, y, z);
        if (_viewDir.abs() - 1 > 1e-6) {
            cerr << "View Direction is not unit vector" << endl;
            return false;
        }
        // Setting scene variable
        this->viewDir = _viewDir;
        return true;
    }

    bool readUpDir(istringstream &iss) {
        // Value validation
        float x, y, z;
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "Updir coordinates incomplete" << endl;
            return false;
        }
        Vector3D _upDir = Vector3D(x, y, z);
        if (_upDir.abs() - 1 > 1e-6) {
            cerr << "Up Direction is not unit vector" << endl;
            return false;
        }
        // Setting scene variable
        this->upDir = _upDir;
        return true;
    }

    bool readVFov(istringstream &iss) {
        // Value validation
        float vFovDeg;
        if (!(iss >> vFovDeg)) {
            cerr << "VFov incomplete" << endl;
            return false;
        }
        if (vFovDeg <= 0 || vFovDeg >= 180) {
            cerr << "VFov is Invalid" << endl;
            return false;
        }
        // Setting scene variable
        this->vFovDeg = vFovDeg;
        return true;
    }

    bool readImageSize(istringstream &iss) {
        // Value validation
        int _imWidth, _imHeight;
        if (!(iss >> _imWidth) || !(iss >> _imHeight)) {
            cerr << "Image size incomplete" << endl;
            return false;
        }
        if (_imWidth <= 0 || _imHeight <= 0) {
            cerr << "Image size invalid" << endl;
            return false;
        }
        // Setting scene variable
        this->imWidth = _imWidth;
        this->imHeight = _imHeight;
        return true;
    }

    bool readBgColor(istringstream &iss) {
        // Value validation
        float r, g, b;
        if (!(iss >> r) || !(iss >> g) || !(iss >> b)) {
            cerr << "Background color incomplete" << endl;
            return false;
        }
        if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) {
            cerr << "Background color component is not between 0 and 1" << endl;
            return false;
        }
        // Setting scene variable
        this->bgColor = Color(r, g, b);
        return true;
    }

    bool readMtlColor(istringstream &iss, MaterialColor &color) {
        float dr, dg, db, sr, sg, sb, ka, kd, ks;
        int n;
        // Od validation
        if (!(iss >> dr) || !(iss >> dg) || !(iss >> db)) {
            cerr << "Material color Od incomplete" << endl;
            return false;
        }
        if (dr < 0 || dr > 1 || dg < 0 || dg > 1 || db < 0 || db > 1) {
            cerr << "Material color Od component is not between 0 and 1" << endl;
            return false;
        }
        // Os validation
        if (!(iss >> sr) || !(iss >> sg) || !(iss >> sb)) {
            cerr << "Material color Os incomplete" << endl;
            return false;
        }
        if (sr < 0 || sr > 1 || sg < 0 || sg > 1 || sb < 0 || sb > 1) {
            cerr << "Material color Os component is not between 0 and 1" << endl;
            return false;
        }
        // Ks validation
        if (!(iss >> ka) || !(iss >> kd) || !(iss >> ks)) {
            cerr << "Material color Ks incomplete" << endl;
            return false;
        }
        if (ka < 0 || ka > 1 || kd < 0 || kd > 1 || ks < 0 || ks > 1) {
            cerr << "Material color K component is not between 0 and 1" << endl;
            return false;
        }
        // n validation
        if (!(iss >> n)) {
            cerr << "Material color n incomplete" << endl;
            return false;
        }
        if (n < 0) {
            cerr << "Material color n is negative" << endl;
            return false;
        }
        // update state variable material color
        color = MaterialColor(Color(dr, dg, db), Color(sr, sg, sb), ka, kd, ks, n);
        return true;
    }

    bool readSphere(istringstream &iss, MaterialColor &color) {
        float x, y, z, rad;
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "Sphere coordinates incomplete" << endl;
            return false;
        }
        if (!(iss >> rad)) {
            cerr << "Sphere radius incomplete" << endl;
            return false;
        }
        if (rad <= 0) {
            cerr << "Sphere radius is non-positive" << endl;
            return false;
        }
        // Setting scene variable
        this->spheres.emplace_back(Vector3D(x, y, z), rad, color);
        return true;
    }

    bool readEllipsoid(istringstream &iss, MaterialColor &color) {
        float x, y, z, rx, ry, rz;
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "Ellipsoid coordinates incomplete" << endl;
            return false;
        }
        if (!(iss >> rx) || !(iss >> ry) || !(iss >> rz)) {
            cerr << "Ellipsoid radius incomplete" << endl;
            return false;
        }
        if (rx <= 0 || ry <= 0 || rz <= 0) {
            cerr << "Ellipsoid radius is non-positive" << endl;
            return false;
        }
        // Setting scene variable
        this->ellipsoids.emplace_back(Vector3D(x, y, z), rx, ry, rz, color);
        return true;
    }

    bool readLight(istringstream &iss) {
        float r, g, b, w, x, y, z;
        // (x, y, z) validation
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "Light (x, y, z) incomplete" << endl;
            return false;
        }
        if (!(iss >> w)) {
            cerr << "Light w incomplete" << endl;
            return false;
        }
        // w validation
        if (w != 0 && w != 1) {
            cerr << "Light w is invalid" << endl;
            return false;
        }
        // (r, g, b) validation
        if (!(iss >> r) || !(iss >> g) || !(iss >> b)) {
            cerr << "Light (r, g, b) incomplete" << endl;
            return false;
        }
        if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) {
            cerr << "Light r is not between 0 and 1" << endl;
            return false;
        }
        // Setting scene variable
        Light light(Vector3D(x, y, z), (int) w, Color(r, g, b));
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
