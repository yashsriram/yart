#ifndef ELLIPSOID_HPP
#define ELLIPSOID_HPP

#include "color.hpp"

class Ellipsoid {
public:
    Vector3D center;
    float rx, ry, rz;
    MaterialColor color;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Ellipsoid &);

    Ellipsoid() : center(Vector3D()), rx(0), ry(0), rz(0), color(MaterialColor()) {}

    Ellipsoid(Vector3D center, float rx, float ry, float rz, MaterialColor color) : center(center), rx(rx), ry(ry),
                                                                                    rz(rz), color(color) {}

};

// this is to easily print a given object in a well-formatted manner to std for debugging
std::ostream &operator<<(std::ostream &out, const Ellipsoid &s) {
    out << "Ellipsoid: " << s.center << "\t" << s.rx << " " << s.ry << " " << s.rz;
    return out;
}

#endif
