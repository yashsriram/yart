#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "color.hpp"

enum SphereRenderType {
    TEXTURE_LESS,
    TEXTURED,
};

class Sphere {
public:
    const Vector3D center;
    const float radius;
    const MaterialColor materialColor;
    const SphereRenderType renderType;
    const int textureIndex;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Sphere &);

    Sphere(Vector3D center, float radius, MaterialColor color)
            : renderType(TEXTURE_LESS), center(center), radius(radius), materialColor(color),
              textureIndex(-1) {}

    Sphere(Vector3D center, float radius, MaterialColor color, int textureIndex)
            : renderType(TEXTURED), center(center), radius(radius), materialColor(color),
              textureIndex(textureIndex) {}

};

std::ostream &operator<<(std::ostream &out, const Sphere &s) {
    out << "Sphere:" << "\t" << s.center << "\t" << s.radius << "\t" << s.materialColor;
    return out;
}

#endif
