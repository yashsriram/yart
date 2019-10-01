class Light {
public:
    Vector3D vector;
    int type;
    Color color;
    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Light &);

    Light(Vector3D vector, int w, Color color) : vector(vector), type(w), color(color) {}
};

std::ostream &operator<<(std::ostream &out, const Light &l) {
    out << "Light:" << "\t";
    out << l.vector << "\t";
    out << l.type << "\t";
    out << l.color << "\t";
    return out;
}
