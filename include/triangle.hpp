#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "color.hpp"
#include "texturecoordinates.hpp"
#include <vector>

using namespace std;

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
    const TextureCoordinates t1, t2, t3;
    const MaterialColor materialColor;
    const int textureIndex;
    const Vector3D surfaceNormal;
    const float D;
    const float area;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Triangle &);

    // FLAT_TEXTURE_LESS: Concisely initialized: surface normal = (v2 - v1) cross (v3 - v1), D = - (v1 dot normal), area
    Triangle(Vector3D v1, Vector3D v2, Vector3D v3,
             MaterialColor materialColor)
            : type(FLAT_TEXTURE_LESS),
              v1(v1), v2(v2), v3(v3),
              n1(Vector3D()), n2(Vector3D()), n3(Vector3D()),
              t1(TextureCoordinates()), t2(TextureCoordinates()), t3(TextureCoordinates()),
              textureIndex(-1),
              materialColor(materialColor),
              surfaceNormal((v2 - v1).cross(v3 - v1)),
              D(-v1.dot(surfaceNormal)),
              area((v2 - v1).cross(v3 - v1).abs() / 2) {}

    // SMOOTH_TEXTURE_LESS: Concisely initialized: surface normal = (v2 - v1) cross (v3 - v1), D = - (v1 dot normal), area
    Triangle(Vector3D v1, Vector3D v2, Vector3D v3,
             MaterialColor materialColor,
             Vector3D n1, Vector3D n2, Vector3D n3)
            : type(SMOOTH_TEXTURE_LESS),
              v1(v1), v2(v2), v3(v3),
              n1(n1), n2(n2), n3(n3),
              t1(TextureCoordinates()), t2(TextureCoordinates()), t3(TextureCoordinates()),
              textureIndex(-1),
              materialColor(materialColor),
              surfaceNormal((v2 - v1).cross(v3 - v1)),
              D(-v1.dot(surfaceNormal)),
              area((v2 - v1).cross(v3 - v1).abs() / 2) {}

    // FLAT_TEXTURED: Concisely initialized: surface normal = (v2 - v1) cross (v3 - v1), D = - (v1 dot normal), area
    Triangle(Vector3D v1, Vector3D v2, Vector3D v3,
             MaterialColor materialColor,
             TextureCoordinates t1, TextureCoordinates t2, TextureCoordinates t3,
             int textureIndex)
            : type(FLAT_TEXTURED),
              v1(v1), v2(v2), v3(v3),
              n1(Vector3D()), n2(Vector3D()), n3(Vector3D()),
              t1(t1), t2(t2), t3(t3),
              textureIndex(textureIndex),
              materialColor(materialColor),
              surfaceNormal((v2 - v1).cross(v3 - v1)),
              D(-v1.dot(surfaceNormal)),
              area((v2 - v1).cross(v3 - v1).abs() / 2) {}

    // SMOOTH_TEXTURED: Concisely initialized: surface normal = (v2 - v1) cross (v3 - v1), D = - (v1 dot normal), area
    Triangle(Vector3D v1, Vector3D v2, Vector3D v3,
             MaterialColor materialColor,
             Vector3D n1, Vector3D n2, Vector3D n3,
             TextureCoordinates t1, TextureCoordinates t2, TextureCoordinates t3,
             int textureIndex)
            : type(SMOOTH_TEXTURED),
              v1(v1), v2(v2), v3(v3),
              n1(n1), n2(n2), n3(n3),
              t1(t1), t2(t2), t3(t3),
              textureIndex(textureIndex),
              materialColor(materialColor),
              surfaceNormal((v2 - v1).cross(v3 - v1)),
              D(-v1.dot(surfaceNormal)),
              area((v2 - v1).cross(v3 - v1).abs() / 2) {}

    // For smooth triangles returns interpolated normal given point of intersection
    Vector3D getInterpolatedNormal(const Vector3D &poi) const {
        Triangle a(poi, v2, v3, materialColor);
        Triangle b(poi, v1, v3, materialColor);
        Triangle c(poi, v1, v2, materialColor);
        float alpha = a.area / area;
        float beta = b.area / area;
        float gamma = c.area / area;
        return (n1 * alpha + n2 * beta + n3 * gamma).unit();
    }

};

std::ostream &operator<<(std::ostream &out, const Triangle &t) {
    string type;
    if (t.type == FLAT_TEXTURE_LESS) {
        type = "FLAT_TEXTURE_LESS";
    } else if (t.type == FLAT_TEXTURED) {
        type = "FLAT_TEXTURED";
    } else if (t.type == SMOOTH_TEXTURE_LESS) {
        type = "SMOOTH_TEXTURE_LESS";
    } else {
        type = "SMOOTH_TEXTURED";
    }
    out << "TriAng: "
        << type << "\n"
        << "\t" << t.v1 << "\t" << t.v2 << "\t" << t.v3 << "\n"
        << "\t" << t.n1 << "\t" << t.n2 << "\t" << t.n3 << "\n"
        << "\t" << "Surface normal: " << "\t" << t.surfaceNormal << "\t" << "D: " << "\t" << t.D << "\n"
        << "\t" << "TextureIndex: " << "\t" << t.textureIndex << "\n"
        << "\t" << t.materialColor;
    return out;
}

#endif
