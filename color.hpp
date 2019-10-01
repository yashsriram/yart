#ifndef COLOR_HPP
#define COLOR_HPP

class BackgroundColor {
public:
  float r, g, b;
  // this is to easily print a given object to std for debugging
  friend std::ostream& operator<<(std::ostream&, const BackgroundColor&);

  BackgroundColor(): r(0), g(0), b(0) {}
  
  BackgroundColor(float r, float g, float b): r(r), g(g), b(b) {
    if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) {
      throw "Unnormalzied color initialized";
    }
  }

  BackgroundColor operator*(float t) const {
    return BackgroundColor(this->r * t, this->g * t, this->b * t);
  }

  std::string print256Scale() {
   return std::to_string(int(r * 255))
          + " "
          + std::to_string(int(g * 255))
          + " "
          + std::to_string(int(b * 255));
  }

};

class MaterialColor {
public:
    float r, g, b;
    // this is to easily print a given object to std for debugging
    friend std::ostream& operator<<(std::ostream&, const MaterialColor&);

    MaterialColor(): r(0), g(0), b(0) {}

    MaterialColor(float r, float g, float b): r(r), g(g), b(b) {
        if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) {
            throw "Unnormalzied color initialized";
        }
    }

    MaterialColor operator*(float t) const {
        return MaterialColor(this->r * t, this->g * t, this->b * t);
    }

    std::string print256Scale() {
        return std::to_string(int(r * 255))
               + " "
               + std::to_string(int(g * 255))
               + " "
               + std::to_string(int(b * 255));
    }

};

// this is to easily print a given object in a well-formatted manner to std for debugging
std::ostream& operator<<(std::ostream &out, const BackgroundColor &c) {
  out << "(" << c.r << ", " << c.g << ", " << c.b << ")";
  return out;
}

// this is to easily print a given object in a well-formatted manner to std for debugging
std::ostream& operator<<(std::ostream &out, const MaterialColor &c) {
    out << "(" << c.r << ", " << c.g << ", " << c.b << ")";
    return out;
}

#endif
