// Returns smallest positive t (ray parameter) if intersection does occurs in-front of the eye else returns -1
float smallestNonNegativeT(const Ray &ray, const Sphere &sphere, float grace) {
    // A = xd^2 + yd^2 + zd^2 = 1
    float A = 1;
    // B = xdxe + ... - xdxc - ...
    float B = 2 * (ray.direction.dot(ray.eye) - ray.direction.dot(sphere.center));
    // C = xe^2 + ... + xc^2 + ... - 2 * (xe*xc + ...) - rad^2
    float C = ray.eye.absSquare() + sphere.center.absSquare() - 2 * ray.eye.dot(sphere.center) -
              sphere.radius * sphere.radius;
    float discriminant = B * B - 4 * A * C;

    // ray does not intersect sphere
    if (discriminant < 0) {
        return -1;
    }

    float t1 = (-B + sqrt(discriminant)) / (2 * A);
    float t2 = (-B - sqrt(discriminant)) / (2 * A);

    // ray intersects at one point i.e. t1 == t2
    if (discriminant == 0) {
        // if t1 == t2 == non-negative return it
        if (t1 >= grace) {
            return t1;
        }
        // else return -1 to indicate behind eye intersection
        return -1;
    }

    // discriminant > 0 i.e. ray intersects at two points
    if (t1 < grace && t2 < grace) {
        return -1;
    } else if (t1 >= grace && t2 < grace) {
        return t1;
    } else if (t2 >= grace && t1 < grace) {
        return t2;
    } else {
        return min(t1, t2);
    }
}

// Returns smallest positive t (ray parameter) if intersection does occurs in-front of the eye else returns -1
float smallestNonNegativeT(const Ray &ray, const Triangle &triangle, float grace) {
    return -1;
}
