#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <unordered_map>
#include <sstream>
#include "light.hpp"

using namespace std;

// Returns tokens after splitting input string with delimiter
vector<string> split(const string &input, const string &delimiter) {
    string inputCopy = input;
    vector<string> ans;
    int pos = 0;
    string token;
    while ((pos = inputCopy.find(delimiter)) != string::npos) {
        token = inputCopy.substr(0, pos);
        ans.emplace_back(token);
        inputCopy.erase(0, pos + delimiter.length());
    }
    ans.emplace_back(inputCopy);
    return ans;
}

class Scene {
public:
    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Sphere &);

    const string filename;

    // Scene critical
    Vector3D eye;
    Vector3D viewDir;
    Vector3D upDir;
    float vFovDeg;
    int imWidth;
    int imHeight;
    Color bgColor;

    // Scene optional
    vector<Sphere> spheres;
    vector<Triangle> triangles;

    vector<Light> lights;

    Scene(const string &filename) : filename(filename),
                                    eye(Vector3D()), viewDir(Vector3D()), upDir(Vector3D()),
                                    vFovDeg(0), imWidth(0), imHeight(0),
                                    bgColor(Color()) {}

    // Reads the scene description and validates it
    // If everything is valid returns true else returns false and prints and error message
    // If true is returned the scene description is stored in object variables
    bool parse() {
        // Opening input file
        ifstream input(this->filename.c_str());

        if (input.fail()) {
            // Error handling: if file could not be opened
            cerr << "Input file named \"" << this->filename
                 << "\" could not be opened. Maybe it doesn't exist or has insufficient permissions." << endl;
            return false;
        }

        // Checks to ensure critical data is given
        unordered_map<string, int> criticalInputCheck;
        criticalInputCheck["eye"] = 0;
        criticalInputCheck["viewdir"] = 0;
        criticalInputCheck["updir"] = 0;
        criticalInputCheck["vfov"] = 0;
        criticalInputCheck["imsize"] = 0;
        criticalInputCheck["bkgcolor"] = 0;

        MaterialColor materialColor;
        vector<Vector3D> vertices;
        vector<Vector3D> normals;
        bool materialColorExists = false;

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
                if (!this->parseEye(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "viewdir") {
                if (!this->parseViewDir(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "updir") {
                if (!this->parseUpDir(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "vfov") {
                if (!this->parseVFov(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "imsize") {
                if (!this->parseImageSize(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            } else if (keyword == "bkgcolor") {
                if (!this->parseBgColor(iss)) {
                    input.close();
                    return false;
                }
                criticalInputCheck[keyword] = 1;
            }
                // Non-critical or optional input
            else if (keyword == "mtlcolor") {
                if (!this->parseMtlColor(iss, materialColor)) {
                    input.close();
                    return false;
                }
                materialColorExists = true;
            } else if (keyword == "sphere") {
                if (!materialColorExists) {
                    cerr << "Sphere information found without preceding mtl color" << endl;
                    return false;
                }
                if (!this->parseSphere(iss, materialColor)) {
                    input.close();
                    return false;
                }
            } else if (keyword == "v") {
                if (!this->parseVertex(iss, vertices)) {
                    input.close();
                    return false;
                }
            } else if (keyword == "vn") {
                if (!this->parseNormal(iss, normals)) {
                    input.close();
                    return false;
                }
            } else if (keyword == "f") {
                if (!this->parseFace(iss, vertices, normals, materialColor)) {
                    input.close();
                    return false;
                }
            } else if (keyword == "light") {
                if (!this->parseLight(iss)) {
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
    bool parseEye(istringstream &iss) {
        // Validation
        float x, y, z;
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "Eye coordinates incomplete" << endl;
            return false;
        }
        // Setting scene variable
        this->eye = Vector3D(x, y, z);
        return true;
    }

    bool parseViewDir(istringstream &iss) {
        // Validation
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

    bool parseUpDir(istringstream &iss) {
        // Validation
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

    bool parseVFov(istringstream &iss) {
        // Validation
        float _vFovDeg;
        if (!(iss >> _vFovDeg)) {
            cerr << "VFov incomplete" << endl;
            return false;
        }
        if (_vFovDeg <= 0 || _vFovDeg >= 180) {
            cerr << "VFov is Invalid" << endl;
            return false;
        }
        // Setting scene variable
        this->vFovDeg = _vFovDeg;
        return true;
    }

    bool parseImageSize(istringstream &iss) {
        // Validation
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

    bool parseBgColor(istringstream &iss) {
        // Validation
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

    bool parseMtlColor(istringstream &iss, MaterialColor &color) {
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

    bool parseSphere(istringstream &iss, const MaterialColor &color) {
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

    bool parseVertex(istringstream &iss, vector<Vector3D> &vertices) {
        // Validation
        float x, y, z;
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "Vertex coordinates incomplete" << endl;
            return false;
        }
        // Setting scene variable
        vertices.emplace_back(Vector3D(x, y, z));
        return true;
    }

    bool parseNormal(istringstream &iss, vector<Vector3D> &normals) {
        // Validation
        float x, y, z;
        if (!(iss >> x) || !(iss >> y) || !(iss >> z)) {
            cerr << "Normal coordinates incomplete" << endl;
            return false;
        }
        // Setting scene variable
        normals.emplace_back(Vector3D(x, y, z));
        return true;
    }

    // Returns a four sized vector of ints specifying face type, vertex, texture and normal indices
    // If any index is not specified 0 is returned (all valid indices are >= 1 anyway)
    vector<int> parseFaceVertex(const string &vertex) {
        vector<int> ans;
        vector<string> tokens = split(vertex, "/");
        if (tokens.size() == 1) {
            // Flat-shaded texture-less
            ans.emplace_back(FLAT_TEXTURE_LESS);
            ans.emplace_back(stoi(tokens[0]));
            ans.emplace_back(0);
            ans.emplace_back(0);
            return ans;
        } else if (tokens.size() == 2) {
            // Flat-shaded textured
            ans.emplace_back(FLAT_TEXTURED);
            ans.emplace_back(stoi(tokens[0]));
            ans.emplace_back(stoi(tokens[1]));
            ans.emplace_back(0);
            return ans;
        } else if (tokens.size() == 3 && tokens[1].empty()) {
            // Smooth-shaded texture-less
            ans.emplace_back(SMOOTH_TEXTURE_LESS);
            ans.emplace_back(stoi(tokens[0]));
            ans.emplace_back(0);
            ans.emplace_back(stoi(tokens[2]));
            return ans;
        } else if (tokens.size() == 3 && !tokens[1].empty()) {
            // Smooth-shaded textured
            ans.emplace_back(SMOOTH_TEXTURED);
            ans.emplace_back(stoi(tokens[0]));
            ans.emplace_back(stoi(tokens[1]));
            ans.emplace_back(stoi(tokens[2]));
            return ans;
        } else {
            throw "Unknown vertex format";
        }
    }

    bool parseFace(istringstream &iss,
                   const vector<Vector3D> &vertices,
                   const vector<Vector3D> &normals,
                   const MaterialColor &materialColor) {
        // Validation
        string s1, s2, s3;
        if (!(iss >> s1) || !(iss >> s2) || !(iss >> s3)) {
            cerr << "Face indices incomplete" << endl;
            return false;
        }
        // Parsing indices of face
        int v1, v2, v3;
        int n1, n2, n3;
        try {
            vector<int> t1_v1_t1_n1 = parseFaceVertex(s1);
            vector<int> t2_v2_t2_n2 = parseFaceVertex(s2);
            vector<int> t3_v3_t3_n3 = parseFaceVertex(s3);
            // Type consistency validation
            if (t1_v1_t1_n1[0] != t2_v2_t2_n2[0] || t2_v2_t2_n2[0] != t3_v3_t3_n3[0]) {
                throw "Inconsistent face definition";
            }
            // Vertex validation
            v1 = t1_v1_t1_n1[1] - 1;
            v2 = t2_v2_t2_n2[1] - 1;
            v3 = t3_v3_t3_n3[1] - 1;
            // Bounds validation
            if (min(v1, min(v2, v3)) < 0 || max(v1, max(v2, v3)) >= vertices.size()) {
                throw "Face indices out of bounds";
            }
            // Coincident vertex validation (simultaneously takes care of zero area triangles)
            if (v1 == v2 || v2 == v3 || v3 == v1) {
                throw "Some of the face vertices are same";
            }
            if (vertices[v1] == vertices[v2] || vertices[v2] == vertices[v3] || vertices[v3] == vertices[v1]) {
                throw "Some of the face vertices are same";
            }
            switch (t1_v1_t1_n1[0]) {
                case FLAT_TEXTURE_LESS:
                    // Setting scene variable
                    this->triangles.emplace_back(
                            Triangle(vertices[v1], vertices[v2], vertices[v3],
                                     materialColor)
                    );
                    break;
                case FLAT_TEXTURED:
                    // todo
                    break;
                case SMOOTH_TEXTURE_LESS:
                    n1 = t1_v1_t1_n1[3] - 1;
                    n2 = t2_v2_t2_n2[3] - 1;
                    n3 = t3_v3_t3_n3[3] - 1;
                    if (min(n1, min(n2, n3)) < 0 || max(n1, max(n2, n3)) >= normals.size()) {
                        throw "Normal indices out of bounds";
                    }
                    this->triangles.emplace_back(
                            Triangle(vertices[v1], vertices[v2], vertices[v3],
                                     normals[v1], normals[v2], normals[v3],
                                     materialColor)
                    );
                    break;
                case SMOOTH_TEXTURED:
                    // todo
                    break;
                default:
                    throw "Unknown face type";
            }
        } catch (exception &e) {
            cerr << "Error while parsing face: " << e.what() << endl;
            return false;
        } catch (const char *e) {
            cerr << e << endl;
            return false;
        }
        return true;
    }

    bool parseLight(istringstream &iss) {
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
            cerr << "Light color is not between 0 and 1" << endl;
            return false;
        }
        if (x == 0 && Vector3D(x, y, z).abs() < 1e-6) {
            cerr << "Directional light direction vector is zero" << endl;
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
    for (const Triangle &triangle: s.triangles) {
        out << triangle << endl;
    }
    for (const Light &light: s.lights) {
        out << light << endl;
    }
    return out;
}

#endif
