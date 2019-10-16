#ifndef TEXTURE_COORDINATES_HPP
#define TEXTURE_COORDINATES_HPP

class TextureCoordinates {
public:
    const float u;
    const float v;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const TextureCoordinates &);

    TextureCoordinates() : u(0), v(0) {}

    TextureCoordinates(float u, float v) : u(u), v(v) {}

    TextureCoordinates operator+(const TextureCoordinates &b) const {
        return TextureCoordinates(this->u + b.u, this->v + b.v);
    }

    TextureCoordinates operator*(float t) const {
        return TextureCoordinates(this->u * t, this->v * t);
    }


};

std::ostream &operator<<(std::ostream &out, const TextureCoordinates &tc) {
    out << "Texture coor:" << "\t(" << tc.u << ", " << tc.v << ")";
    return out;
}

#endif
