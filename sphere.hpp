#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "color.hpp"

class Sphere {
public:
  Vector3D center;
  float radius;
  NormalizedColor color;

  // this is to easily print a given object to std for debugging
  friend std::ostream& operator<<(std::ostream&, const Sphere&);

  Sphere(): center(Vector3D()), radius(0), color(NormalizedColor())  {}

  Sphere(Vector3D center, float radius, NormalizedColor color): center(center), radius(radius), color(color)  {}

};

// this is to easily print a given object in a well-formatted manner to std for debugging
std::ostream& operator<<(std::ostream &out, const Sphere &s) {
  out << "center = " << s.center << "\n" << "radius = " << s.radius; 
  return out;
}

#endif
