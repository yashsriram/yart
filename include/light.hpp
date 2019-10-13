#include <cstdio>
#include <cstdlib>
#include <ctime>

float getRand() {
    return ((float) (rand() % 100) / 100);
}

class Light {
public:
    const Vector3D vector;
    const int type;
    const Color color;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Light &);

    Light(Vector3D vector, int w, Color color) : vector(vector), type(w), color(color) {}

    Vector3D poiToLightUnitVector(const Vector3D &poi, float jitter = 0) const {
        if (type == 0) {
            return (vector + Vector3D(getRand(), getRand(), getRand()).unit() * jitter * 1e-1) * -1;
        } else {
            return (vector + Vector3D(getRand(), getRand(), getRand()).unit() * jitter - poi).unit();
        }
    }

};

std::ostream &operator<<(std::ostream &out, const Light &l) {
    out << "Light:" << "\t";
    out << l.vector << "\t";
    out << l.type << "\t";
    out << l.color << "\t";
    return out;
}
