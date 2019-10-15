#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "color.hpp"

enum TriangleRenderType {
    FLAT_TEXTURE_LESS,
    FLAT_TEXTURED,
    SMOOTH_TEXTURE_LESS,
    SMOOTH_TEXTURED,
};

class Triangle {
public:
    const TriangleRenderType type;
    const Vector3D v1, v2, v3;
    const Vector3D n1, n2, n3;
    const MaterialColor materialColor;
    const Vector3D surfaceNormal;
    const float D;
    const float area;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Triangle &);

    // Concisely initialized: surface normal = (v2 - v1) cross (v3 - v1), D = - (v1 dot normal)
    Triangle(Vector3D v1, Vector3D v2, Vector3D v3,
             MaterialColor materialColor)
            : type(FLAT_TEXTURE_LESS),
              v1(v1), v2(v2), v3(v3),
              n1(Vector3D()), n2(Vector3D()), n3(Vector3D()),
              materialColor(materialColor),
              surfaceNormal((v2 - v1).cross(v3 - v1)),
              D(-v1.dot(surfaceNormal)),
              area((v2 - v1).cross(v3 - v1).abs() / 2) {}

    // Concisely initialized: surface normal = (v2 - v1) cross (v3 - v1), D = - (v1 dot normal)
    Triangle(Vector3D v1, Vector3D v2, Vector3D v3,
             Vector3D n1, Vector3D n2, Vector3D n3,
             MaterialColor materialColor)
            : type(SMOOTH_TEXTURE_LESS),
              v1(v1), v2(v2), v3(v3),
              n1(n1), n2(n2), n3(n3),
              materialColor(materialColor),
              surfaceNormal((v2 - v1).cross(v3 - v1)),
              D(-v1.dot(surfaceNormal)),
              area((v2 - v1).cross(v3 - v1).abs() / 2) {}

};

std::ostream &operator<<(std::ostream &out, const Triangle &t) {
    out << "TriAng:"
        << "\t" << t.v1 << "\t" << t.v2 << "\t" << t.v3 << "\n"
        << "\t" << "Surface normal: " << "\t" << t.surfaceNormal << "\t" << "D: " << "\t" << t.D << "\n"
        << "\t" << t.materialColor;
    return out;
}

#endif
