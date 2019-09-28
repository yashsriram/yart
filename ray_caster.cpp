#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "vector3d.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "ellipsoid.hpp"
#include "scene.hpp"

using namespace std;

#ifndef M_PI
  #define M_PI 3.1415926535
#endif

// returns smallest positive t (ray parameter) if intersection does occur and it is infront of the eye
// else return -1
float smallestPositiveT(const Ray& ray, const Sphere& sphere) {
  float A = 1; // xd^2 + yd^2 + zd^2 = 1 always
  float B = 2 * (ray.direction.dot(ray.eye) - ray.direction.dot(sphere.center)); // xdxe + ... - xdxc - ...
  float C = ray.eye.absSquare() + sphere.center.absSquare() - 2 * (ray.eye.dot(sphere.center)) - (sphere.radius * sphere.radius); // xe^2 + ... + xc^2 + ... - 2 * (xe*xc + ...) - rad^2
  float discriminant = B * B - 4 * A * C;
  if (discriminant < 0) {
    // ray doesnot intersect sphere
    return -1;
  }

  float t1 = (-B + sqrt(discriminant)) / (2 * A);
  float t2 = (-B - sqrt(discriminant)) / (2 * A);

  // ray intersects at one point i.e. t1 == t2
  if (discriminant == 0) {
    if (t1 >= 0) {
      // if t1 == t2 == non-negative return it
      return t1;
    }
    // else return -1 to indicate behind eye intersection
    return -1;
  }

  // discriminant > 0 i.e. ray intersects at two points
  if (t1 < 0 && t2 < 0) {
    return -1;
  } else if (t1 >= 0 && t2 < 0) {
    return t1;
  } else if (t2 >= 0 && t1 < 0) {
    return t2;
  } else {
    return min(t1, t2);
  }
}

// returns smallest positive t (ray parameter) if intersection does occur and it is infront of the eye
// else return -1
float smallestPositiveT(const Ray& ray, const Ellipsoid& ellipsoid) {
  Vector3D d = ray.direction;
  Vector3D e = ray.eye;
  Vector3D cen = ellipsoid.center;
  float a = ellipsoid.rx;
  float b = ellipsoid.ry;
  float c = ellipsoid.rz;
  float A = (d.x / a) * (d.x / a) + (d.y / b) * (d.y / b) + (d.z / c) * (d.z / c);
  float B = 2 * (((e.x * d.x - e.x * cen.x) / (a * a)) + ((e.y * d.y - e.y * cen.y) / (b * b)) + ((e.z * d.z - e.z * cen.z) / (c * c)));
  float C = (cen.x / a) * (cen.x / a) + (cen.y / b) * (cen.y / b) + (cen.z / c) * (cen.z / c)
            + (e.x / a) * (e.x / a) + (e.y / b) * (e.y / b) + (e.z / c) * (e.z / c)
            - 2 * ((e.x * cen.x / (a * a)) + (e.y * cen.y / (b * b)) + (e.z * cen.z / (c * c)))
            - 1;
  float discriminant = B * B - 4 * A * C;
  if (discriminant < 0) {
    // ray doesnot intersect ellipsoid
    return -1;
  }

  float t1 = (-B + sqrt(discriminant)) / (2 * A);
  float t2 = (-B - sqrt(discriminant)) / (2 * A);

  // ray intersects at one point i.e. t1 == t2
  if (discriminant == 0) {
    if (t1 >= 0) {
      // if t1 == t2 == non-negative return it
      return t1;
    }
    // else return -1 to indicate behind eye intersection
    return -1;
  }

  // discriminant > 0 i.e. ray intersects at two points
  if (t1 < 0 && t2 < 0) {
    return -1;
  } else if (t1 >= 0 && t2 < 0) {
    return t1;
  } else if (t2 >= 0 && t1 < 0) {
    return t2;
  } else {
    return min(t1, t2);
  }
}

NormalizedColor traceRay(const Ray& ray, const Scene& scene) {
  int nearestObjIndex = -1;
  float minT = -1;
  NormalizedColor nearestObjColor;
  // Inspect intersection with all spheres
  for (int i = 0; i < scene.spheres.size(); ++i) {
    float t = smallestPositiveT(ray, scene.spheres[i]);
    if (t >= 0) {
      if (nearestObjIndex == -1) {
        nearestObjIndex = i;
        nearestObjColor = scene.spheres[i].color;
        minT = t;
      } else {
        if (t < minT) {
          minT = t;
          nearestObjIndex = i;
          nearestObjColor = scene.spheres[i].color;
        }
      }
    }
  }

  // Inspect intersection with all ellipsoids
  for (int i = 0; i < scene.ellipsoids.size(); ++i) {
    float t = smallestPositiveT(ray, scene.ellipsoids[i]);
    if (t >= 0) {
      if (nearestObjIndex == -1) {
        nearestObjIndex = i;
        nearestObjColor = scene.ellipsoids[i].color;
        minT = t;
      } else {
        if (t < minT) {
          minT = t;
          nearestObjIndex = i;
          nearestObjColor = scene.ellipsoids[i].color;
        }
      }
    }
  }

  if (nearestObjIndex >= 0) {
    return nearestObjColor;
  } else {
    return scene.bgColor * ray.direction.dot(scene.viewDir.unit());
  }
}

int main(int argc, char *argv[]) {
  // Restricting number of commandline arguments
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <inputfile>" << endl;
    exit(-1);
  }
  string filename(argv[1]);

  // Read scene description from input file
  Scene scene(filename);
  if (!scene.readAndValidate()) {
    return -1;
  };

  // Preliminary calculations
  Vector3D u = scene.viewDir.cross(scene.upDir);
  Vector3D v = u.cross(scene.viewDir);
  Vector3D n = u.cross(v);

  float d = scene.imHeight / (2 * tan(scene.vFovDeg * M_PI / 360));

  Vector3D imageCenter = scene.eye + scene.viewDir.unit() * d;
  Vector3D ul = imageCenter - u * (scene.imWidth / 2) + v * (scene.imHeight / 2);
  Vector3D ur = imageCenter + u * (scene.imWidth / 2) + v * (scene.imHeight / 2);
  Vector3D ll = imageCenter - u * (scene.imWidth / 2) - v * (scene.imHeight / 2);
  Vector3D lr = imageCenter + u * (scene.imWidth / 2) - v * (scene.imHeight / 2);

  Vector3D delWidth = (ur - ul) * (1 / (scene.imWidth - 1));
  Vector3D delHeight = (ll - ul) * (1 / (scene.imHeight - 1));

  // Initialize pixel array for output image
  vector<vector<NormalizedColor> > colors;
  for (int i = 0; i < scene.imWidth; i++) {
    vector<NormalizedColor> col;
    col.resize(scene.imHeight);
    colors.push_back(col);
  }

  // Ray tracing per pixel
  for (int j = 0; j < scene.imHeight; j++) {
    for (int i = 0; i < scene.imWidth; i++) {
      // (i, j) pixel coordinate
      Vector3D pixelCoordinate = ul + delWidth * i + delHeight * j;
      // ray from eye to that pixel
      Ray ray(scene.eye, (pixelCoordinate - scene.eye).unit());
      // trace this ray in the scene to produce a color for the pixel
      NormalizedColor pixelValue = traceRay(ray, scene);
      // Keep track of color
      colors[i][j] = pixelValue;
    }
  }


  // Write the final image to an output file
  string outputfileString(filename);
  int len = outputfileString.size();
  outputfileString.replace(len - 3, 3, "ppm");

  // Creating PPM image
  ofstream outputfile;
  outputfile.open(outputfileString);

  // Filling in the header
  outputfile << "P3" << endl << "# image autogenerated using a simple ray tracer" << endl;
  outputfile << scene.imWidth << " " << scene.imHeight << endl;
  outputfile << 255 << endl;

  // Filling in the body
  for (int j = 0; j < scene.imHeight; ++j) {
    for (int i = 0; i < scene.imWidth; ++i) {
      // Write the R G B values of a pixel in seperate line
      outputfile << colors[i][j].print256Scale() << endl;
    }
  }

  // Closing the output stream
  outputfile.close();

  return 0;
}
