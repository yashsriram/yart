#ifndef COLOR_HPP
#define COLOR_HPP

class Color {
    float r, g, b;
public:

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Color &);

    Color() : r(0), g(0), b(0) {}

    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    Color operator*(float t) const {
        return Color(this->r * t, this->g * t, this->b * t);
    }

    Color operator+(const Color &B) const {
        return Color(this->r + B.r, this->g + B.g, this->b + B.b);
    }

    Color operator*(const Color &B) const {
        return Color(this->r * B.r, this->g * B.g, this->b * B.b);
    }

    std::string to8BitScale() const {
        // Clamp color channels
        float R = r, G = g, B = b;
        if (r < 0) { R = 0; } else if (r > 1) { R = 1; }
        if (g < 0) { G = 0; } else if (g > 1) { G = 1; }
        if (b < 0) { B = 0; } else if (b > 1) { B = 1; }
        // Return appropriate string value
        return std::to_string(int(R * 255))
               + " "
               + std::to_string(int(G * 255))
               + " "
               + std::to_string(int(B * 255));
    }

};

class MaterialColor {
public:
    Color diffusion;
    Color specular;
    float ka, kd, ks;
    int n;
    float opacity;
    float refractiveIndex;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const MaterialColor &);

    MaterialColor() : diffusion(0, 0, 0), specular(0, 0, 0), ka(0), kd(0), ks(0), n(0), opacity(0),
                      refractiveIndex(0) {}

    MaterialColor(Color diffusion, Color specular, float ka, float kd, float ks, int n, float opacity,
                  float refractiveIndex)
            : diffusion(diffusion), specular(specular), ka(ka), kd(kd), ks(ks), n(n), opacity(opacity),
              refractiveIndex(refractiveIndex) {}

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
    out << "(" << mtl.opacity << ", " << mtl.refractiveIndex << ")\t";
    return out;
}

#endif
