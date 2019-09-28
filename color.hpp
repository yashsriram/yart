#ifndef COLOR_HPP
#define COLOR_HPP

class NormalizedColor {
public:
  float r, g, b;
  // this is to easily print a given object to std for debugging
  friend std::ostream& operator<<(std::ostream&, const NormalizedColor&);

  NormalizedColor(): r(0), g(0), b(0) {}
  
  NormalizedColor(float r, float g, float b): r(r), g(g), b(b) {
    if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) {
      throw "Unnormalzied color initialized";
    }
  }

  NormalizedColor operator*(float t) const {
    return NormalizedColor(this->r * t, this->g * t, this->b * t);
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
std::ostream& operator<<(std::ostream &out, const NormalizedColor &c) {
  out << "(" << c.r << ", " << c.g << ", " << c.b << ")";
  return out;
}

#endif
