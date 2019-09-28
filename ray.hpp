#ifndef RAY_HPP
#define RAY_HPP

class Ray {
public:
  Vector3D eye;
  Vector3D direction;
  // this is to easily print a given object to std for debugging
  friend std::ostream& operator<<(std::ostream&, const Ray&);

  Ray(Vector3D eye, Vector3D direction): eye(eye), direction(direction) {}

};

// this is to easily print a given object in a well-formatted manner to std for debugging
std::ostream& operator<<(std::ostream &out, const Ray &r) {
  out << "eye = " << r.eye << "\n" << "direction = " << r.direction; 
  return out;
}

#endif
