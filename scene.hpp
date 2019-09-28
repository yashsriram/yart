#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <unordered_map>

using namespace std;

class Scene {
public:
  const string filename;

  Vector3D eye;
  Vector3D viewDir;
  Vector3D upDir;

  float vFovDeg;
  float imWidth;
  float imHeight;

  NormalizedColor bgColor;
  vector<Sphere> spheres;
  vector<Ellipsoid> ellipsoids;

  // Constructor initializes filename
  Scene(const string& filename): filename(filename),
                                 eye(Vector3D()), viewDir(Vector3D()), upDir(Vector3D()),
                                 vFovDeg(0), imWidth(0), imHeight(0),
                                 bgColor(NormalizedColor()) {}

  // Reads the scene description and validates it
  // If everything is valid returns true else returns false and prints and error message
  // If true is returned the scene description is stored in object variables
  bool readAndValidate() {
    // Opening input file
    ifstream input(this->filename.c_str());

    if (input.fail()) {
      // Error handling: if file could not be opened
      cerr << "Input file named \"" << this->filename << "\" could not be opened. Maybe it doesn't exist or has insufficient permissions." << endl;
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
    keywordsCheck["sphere"] = 0;
    NormalizedColor mtlcolor;
    bool mtlcolorExists = false;

    string line;
    while (getline(input, line)) {
      string keyword;
      istringstream iss(line);

      // Keyword validation
      if (!(iss >> keyword)) {
        continue;
      }
      if (keyword == "eye") {
        // Validate eye
        if (!this->validateEye(iss)) {
          input.close();
          return false;
        }
        keywordsCheck[keyword] = 1;
      } else if (keyword == "viewdir") {
        // Validate view dir
        if (!this->validateViewDir(iss)) {
          input.close();
          return false;
        }
        keywordsCheck[keyword] = 1;
      } else if (keyword == "updir") {
        // Validate up dir
        if (!this->validateUpDir(iss)) {
          input.close();
          return false;
        }
        keywordsCheck[keyword] = 1;
      } else if (keyword == "vfov") {
        // Validate vfov
        if (!this->validateVFov(iss)) {
          input.close();
          return false;
        }
        keywordsCheck[keyword] = 1;
      } else if (keyword == "imsize") {
        // Validate im dims
        if (!this->validateImageDim(iss)) {
          input.close();
          return false;
        }
        keywordsCheck[keyword] = 1;
      } else if (keyword == "bkgcolor") {
        // Validate bg color
        if (!this->validateBgColor(iss)) {
          input.close();
          return false;
        }
        keywordsCheck[keyword] = 1;
      } else if (keyword == "mtlcolor") {
        // Validate mtl color
        if (!this->validateMtlColor(iss, mtlcolor)) {
          input.close();
          return false;
        }
        keywordsCheck[keyword] = 1;
        mtlcolorExists = true;
      } else if (keyword == "sphere") {
        // Validate sphere
        if (!mtlcolorExists) {
          cerr << "Syntax error in inputfile \"" << this->filename << "\". Sphere information found without preceeding mtl color" << endl;
          return false;
        }
        if (!this->validateSphere(iss, mtlcolor)) {
          input.close();
          return false;
        }
        keywordsCheck[keyword] = 1;
      } else if (keyword == "ellipsoid") {
        // Validate ellipsoid
        if (!mtlcolorExists) {
          cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information found without preceeding mtl color" << endl;
          return false;
        }
        if (!this->validateEllipsoid(iss, mtlcolor)) {
          input.close();
          return false;
        }
      } else {
        cerr << "Syntax error in inputfile \"" << this->filename << "\". Invalid keyword" << endl;
        return false;
      }
    }

    // Parallel view and up vector check
    if (this->upDir.dot(this->viewDir) == 1 || this->upDir.dot(this->viewDir) == -1) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Parallel/Anti-parallel up and view directions! " << endl;
      input.close();
      return false;
    }

    for (auto i : keywordsCheck) {
      if (i.second != 1) {
        cerr << "Syntax error in inputfile \"" << this->filename << "\". Information missing: " << i.first << endl;
        return false;
      }
    }

    // cout << eye << endl;
    // cout << viewDir << endl;
    // cout << upDir << endl;
    // cout << vFovDeg << endl;
    // cout << imWidth << imHeight << endl;
    // cout << bgColor << endl;
    // for (Sphere & sphere: this->spheres) {
    //   cout << sphere << endl;
    // }

    input.close();
    return true;
  }

private:
    bool validateEye(istringstream &iss) {
    // Value validation
    float x, y, z;
    if (!(iss >> x)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Eye x-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> y)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Eye y-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> z)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Eye z-coordinate Information missing" << endl;
      return false;
    }

    // Setting scene variable
    this->eye = Vector3D(x, y, z);

    return true;
  }

  bool validateViewDir(istringstream &iss) {
    // Value validation
    float x, y, z;
    if (!(iss >> x)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". View Direction x-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> y)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". View Direction y-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> z)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". View Direction z-coordinate Information missing" << endl;
      return false;
    }
    Vector3D viewDir = Vector3D(x, y, z);
    if (viewDir.abs() - 1 > 1e-6) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". View Direction is not unit vector" << endl;
      return false;
    }

    // Setting scene variable
    this->viewDir = viewDir;

    return true;
  }

  bool validateUpDir(istringstream &iss) {
    // Value validation
    float x, y, z;
    if (!(iss >> x)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Up Direction x-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> y)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Up Direction y-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> z)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Up Direction z-coordinate Information missing" << endl;
      return false;
    }
    Vector3D upDir = Vector3D(x, y, z);
    if (upDir.abs() - 1 > 1e-6) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Up Direction is not unit vector" << endl;
      return false;
    }

    // Setting scene variable
    this->upDir = upDir;

    return true;
  }

  bool validateVFov(istringstream &iss) {
    // Value validation
    float vfov;
    if (!(iss >> vfov)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". VFov Information missing" << endl;
      return false;
    }
    if (vfov <= 0 || vfov  >= 180) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". VFov is Invalid" << endl;
      return false;
    }

    // Setting scene variable
    this->vFovDeg = vfov;

    return true;
  }

  bool validateImageDim(istringstream &iss) {
    // Value validation
    int imWidth, imHeight;
    if (!(iss >> imWidth)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Image dimensions width Information missing" << endl;
      return false;
    }
    if (!(iss >> imHeight)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Image dimensions height Information missing" << endl;
      return false;
    }
    if (imWidth <= 0) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Image dimensions width is Invalid" << endl;
      return false;
    }
    if (imHeight <= 0) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Image dimensions width is Invalid" << endl;
      return false;
    }

    // Setting scene variable
    this->imWidth = imWidth;
    this->imHeight = imHeight;

    return true;
  }

  bool validateBgColor(istringstream &iss) {
    // Value validation
    float r, g, b;
    if (!(iss >> r)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Background color r-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> g)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Background color g-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> b)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Background color b-coordinate Information missing" << endl;
      return false;
    }
    if (r < 0 || r > 1) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Background color r is not between 0 and 1" << endl;
      return false;
    }
    if (g < 0 || g > 1) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Background color g is not between 0 and 1" << endl;
      return false;
    }
    if (b < 0 || b > 1) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Background color b is not between 0 and 1" << endl;
      return false;
    }

    // Setting scene variable
    this->bgColor = NormalizedColor(r, g, b);

    return true;
  }

  bool validateMtlColor(istringstream &iss, NormalizedColor& color) {
    float r, g, b;
    if (!(iss >> r)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Material color r-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> g)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Material color g-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> b)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Material color b-coordinate Information missing" << endl;
      return false;
    }
    if (r < 0 || r > 1) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Material color r is not between 0 and 1" << endl;
      return false;
    }
    if (g < 0 || g > 1) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Material color g is not between 0 and 1" << endl;
      return false;
    }
    if (b < 0 || b > 1) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Material color b is not between 0 and 1" << endl;
      return false;
    }
    // update state variable material color
    color.r = r;
    color.g = g;
    color.b = b;

    return true;
  }

  bool validateSphere(istringstream &iss, NormalizedColor color) {
    float x, y, z, rad;
    if (!(iss >> x)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Sphere information x-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> y)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Sphere information y-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> z)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Sphere information z-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> rad)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Sphere information radius Information missing" << endl;
      return false;
    }
    if (rad <= 0) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Sphere information radius is non-positive" << endl;
      return false;
    }

    // Setting scene variable
    this->spheres.push_back(Sphere(Vector3D(x, y, z), rad, color));

    return true;
  }

  bool validateEllipsoid(istringstream &iss, NormalizedColor color) {
    float x, y, z, rx, ry, rz;
    if (!(iss >> x)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information x-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> y)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information y-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> z)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information z-coordinate Information missing" << endl;
      return false;
    }
    if (!(iss >> rx)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information radius Information missing" << endl;
      return false;
    }
    if (!(iss >> ry)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information radius Information missing" << endl;
      return false;
    }
    if (!(iss >> rz)) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information radius Information missing" << endl;
      return false;
    }
    if (rx <= 0) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information radius is non-positive" << endl;
      return false;
    }
    if (ry <= 0) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information radius is non-positive" << endl;
      return false;
    }
    if (rz <= 0) {
      cerr << "Syntax error in inputfile \"" << this->filename << "\". Ellipsoid information radius is non-positive" << endl;
      return false;
    }

    // Setting scene variable
    this->ellipsoids.push_back(Ellipsoid(Vector3D(x, y, z), rx, ry, rz, color));

    return true;
  }

};

#endif
