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

};

std::ostream &operator<<(std::ostream &out, const TextureCoordinates &tc) {
    out << "Texture coor:" << "\t(" << tc.u << ", " << tc.v << ")";
    return out;
}

#endif
