class Light {
    friend std::ostream &operator<<(std::ostream &, const Light &);
public:
    Vector3D vector;
    int type;
    Color color;

    Light(Vector3D vector, int w, Color color) : vector(vector), type(w), color(color) {}
};

std::ostream &operator<<(std::ostream &out, const Light &l) {
    out << "Light:" << "\t";
    out << l.vector << "\t";
    out << l.type << "\t";
    out << l.color << "\t";
    return out;
}
