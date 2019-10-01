#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "color.hpp"

class Sphere {
public:
  Vector3D center;
  float radius;
  MaterialColor materialColor;

  // this is to easily print a given object to std for debugging
  friend std::ostream& operator<<(std::ostream&, const Sphere&);

  Sphere(): center(Vector3D()), radius(0), materialColor(MaterialColor())  {}

  Sphere(Vector3D center, float radius, MaterialColor color): center(center), radius(radius), materialColor(color)  {}

};

std::ostream& operator<<(std::ostream &out, const Sphere &s) {
  out << "Sphere:" << "\t" << s.center << "\t" << s.radius << "\t" << s.materialColor;
  return out;
}

#endif
