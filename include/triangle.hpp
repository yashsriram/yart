#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "color.hpp"

class Triangle {
    Vector3D v1, v2, v3;
    MaterialColor materialColor;
    Vector3D normal;
    float D;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Triangle &);

public:

    Triangle() : v1(Vector3D()), v2(Vector3D()), v3(Vector3D()), materialColor(MaterialColor()), normal(Vector3D()),
                 D(0) {}

    // Concisely initialized
    // normal = (v2 - v1) cross (v3 - v1)
    // D = - (v1 dot normal)
    Triangle(Vector3D v1, Vector3D v2, Vector3D v3, MaterialColor materialColor) : v1(v1), v2(v2), v3(v3),
                                                                                   materialColor(materialColor),
                                                                                   normal((v2 - v1).cross(v3 - v1)),
                                                                                   D(-v1.dot(normal)) {}

};

std::ostream &operator<<(std::ostream &out, const Triangle &t) {
    out << "TriAng:" << "\t" << t.v1 << "\t" << t.v2 << "\t" << t.v3 << "\n"
        << "\t" << "Normal: " << "\t" << t.normal << "\t" << "D: " << "\t" << t.D << "\n"
        << "\t" << t.materialColor;
    return out;
}

#endif
