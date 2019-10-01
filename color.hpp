#ifndef COLOR_HPP
#define COLOR_HPP

class Color {
public:
    float r, g, b;
    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Color &);

    Color() : r(0), g(0), b(0) {}

    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    Color operator*(float t) const {
        return Color(this->r * t, this->g * t, this->b * t);
    }

    std::string to8BitScale() {
        return std::to_string(int(r * 255))
               + " "
               + std::to_string(int(g * 255))
               + " "
               + std::to_string(int(b * 255));
    }

};

class MaterialColor {
public:
    Color diffusion;
    Color specular;
    float ka, kd, ks;
    int n;
    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const MaterialColor &);

    MaterialColor() : diffusion(0, 0, 0), specular(0, 0, 0), ka(0), kd(0), ks(0), n(0) {}

    MaterialColor(Color diffusion, Color specular, float ka, float kd, float ks, int n)
            : diffusion(diffusion), specular(specular), ka(ka), kd(kd), ks(ks), n(n) {}

    std::string to8BitScale() {
        return specular.to8BitScale();
    }

};

std::ostream &operator<<(std::ostream &out, const Color &c) {
    out << "(" << c.r << ", " << c.g << ", " << c.b << ")";
    return out;
}

std::ostream &operator<<(std::ostream &out, const MaterialColor &mtl) {
    out << "Material color:" << "\t";
    out << mtl.diffusion << "\t" << mtl.specular << "\t";
    out << "(" << mtl.ka << ", " << mtl.kd << ", " << mtl.ks << ")\t";
    out << mtl.n;
    return out;
}

#endif
