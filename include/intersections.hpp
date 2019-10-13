// Returns smallest positive t (ray parameter) if intersection does occurs in-front of the origin else returns -1
float smallestNonNegativeT(const Ray &ray, const Sphere &sphere, float grace) {
    // A = xd^2 + yd^2 + zd^2 = 1
    float A = 1;
    // B = xdxe + ... - xdxc - ...
    float B = 2 * (ray.direction.dot(ray.origin) - ray.direction.dot(sphere.center));
    // C = xe^2 + ... + xc^2 + ... - 2 * (xe*xc + ...) - rad^2
    float C = ray.origin.absSquare() + sphere.center.absSquare() - 2 * ray.origin.dot(sphere.center) -
              sphere.radius * sphere.radius;
    float discriminant = B * B - 4 * A * C;

    // Ray does not intersect sphere
    if (discriminant < 0) {
        return -1;
    }

    float t1 = (-B + sqrt(discriminant)) / (2 * A);
    float t2 = (-B - sqrt(discriminant)) / (2 * A);

    // Ray intersects at one point i.e. t1 == t2
    if (discriminant == 0) {
        if (t1 >= grace) {
            // t1 == t2 == non-negative
            return t1;
        }
        // Behind origin intersection
        return -1;
    }

    // Discriminant > 0 i.e. ray intersects at two points
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

// Returns smallest positive t (ray parameter) if intersection does occurs in-front of the origin else returns -1
float smallestNonNegativeT(const Ray &ray, const Triangle &triangle, float grace) {
    float denominator = triangle.normal.dot(ray.direction);
    // Parallel / Coincident ray; doesn't intersect
    if (abs(denominator) < 1e-6) { return -1; }
    float numerator = -(triangle.normal.dot(ray.origin) + triangle.D);
    float t = numerator / denominator;
    // Behind origin intersection
    if (t < 0) { return -1; }
    Vector3D poi = ray.getPoint(t);
    Triangle a(poi, triangle.v2, triangle.v3, triangle.materialColor);
    Triangle b(poi, triangle.v1, triangle.v3, triangle.materialColor);
    Triangle c(poi, triangle.v1, triangle.v2, triangle.materialColor);
    // Out of triangle intersection
    if (a.area + b.area + c.area - triangle.area > 1e-6) { return -1; }
    return t;
}
