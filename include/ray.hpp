#ifndef RAY_HPP
#define RAY_HPP

class Ray {
public:
    const Vector3D origin;
    const Vector3D direction;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Ray &);

    Ray(Vector3D eye, Vector3D direction) : origin(eye), direction(direction) {}

    Vector3D getPoint(float t) const {
        return Vector3D(origin + direction * t);
    }

};

std::ostream &operator<<(std::ostream &out, const Ray &r) {
    out << "Ray:" << "\t" << r.origin << "\t" << r.direction;
    return out;
}

#endif
