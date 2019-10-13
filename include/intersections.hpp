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

// returns smallest positive t (ray parameter) if intersection does occur and it is infront of the eye
// else return -1
float smallestNonNegativeT(const Ray &ray, const Ellipsoid &ellipsoid, float grace) {
    Vector3D d = ray.direction;
    Vector3D e = ray.eye;
    Vector3D cen = ellipsoid.center;
    float a = ellipsoid.rx;
    float b = ellipsoid.ry;
    float c = ellipsoid.rz;
    float A = (d.x / a) * (d.x / a) + (d.y / b) * (d.y / b) + (d.z / c) * (d.z / c);
    float B = 2 * (((e.x * d.x - e.x * cen.x) / (a * a)) + ((e.y * d.y - e.y * cen.y) / (b * b)) +
                   ((e.z * d.z - e.z * cen.z) / (c * c)));
    float C = (cen.x / a) * (cen.x / a) + (cen.y / b) * (cen.y / b) + (cen.z / c) * (cen.z / c)
              + (e.x / a) * (e.x / a) + (e.y / b) * (e.y / b) + (e.z / c) * (e.z / c)
              - 2 * ((e.x * cen.x / (a * a)) + (e.y * cen.y / (b * b)) + (e.z * cen.z / (c * c)))
              - 1;
    float discriminant = B * B - 4 * A * C;
    if (discriminant < 0) {
        // ray doesnot intersect ellipsoid
        return -1;
    }

    float t1 = (-B + sqrt(discriminant)) / (2 * A);
    float t2 = (-B - sqrt(discriminant)) / (2 * A);

    // ray intersects at one point i.e. t1 == t2
    if (discriminant == 0) {
        if (t1 >= grace) {
            // if t1 == t2 == non-negative return it
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
