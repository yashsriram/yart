#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "color.hpp"

class Triangle {
public:
    Vector3D v1, v2, v3;
    MaterialColor materialColor;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Sphere &);

    Triangle() : v1(Vector3D()), v2(Vector3D()), v3(Vector3D()), materialColor(MaterialColor()) {}

    Triangle(Vector3D v1, Vector3D v2, Vector3D v3, MaterialColor materialColor) : v1(v1), v2(v2), v3(v3), materialColor(materialColor) {}

};

std::ostream &operator<<(std::ostream &out, const Triangle &t) {
    out << "TriAng:" << "\t" << t.v1 << "\t" << t.v2 << "\t" << t.v3 << "\t" << t.materialColor;
    return out;
}

#endif
