// Returns index of smallest non-negative number from vector
// If all are negative then returns -1
int indexOfSmallestNonNegativeElement(const vector<float> &vector) {
    float minT = -1;
    int ans = -1;
    for (int i = 0; i < vector.size(); ++i) {
        float t = vector[i];
        // t < 0 => prune
        if (t < 0) {
            continue;
        }

        // t >= 0
        if (ans == -1) {
            ans = i;
            minT = t;
        } else {
            if (t < minT) {
                ans = i;
                minT = t;
            }
        }
    }
    return ans;
}

// Returns smallest positive t (ray parameter) if intersection does occurs in-front of the origin else returns -1
float smallestNonNegativeT(const Ray &ray, const Sphere &sphere, float grace) {
    // A = xd^2 + yd^2 + zd^2 = 1
    float A = ray.direction.absSquare();
    // B = xdxe + ... - xdxc - ...
    float B = 2 * (ray.direction.dot(ray.origin - sphere.center));
    // C = xe^2 + ... + xc^2 + ... - 2 * (xe*xc + ...) - rad^2
    float C = (ray.origin - sphere.center).absSquare() - sphere.radius * sphere.radius;
    float discriminant = (B * B) - (4 * A * C);

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
// Independent of renderType of triangle
float smallestNonNegativeT(const Ray &ray, const Triangle &triangle, float grace) {
    float denominator = triangle.surfaceNormal.dot(ray.direction);
    float numerator = -(triangle.surfaceNormal.dot(ray.origin) + triangle.D);
    // Parallel / Coincident ray; doesn't intersect
    if (abs(denominator) < 1e-6) { return -1; }
    // Behind origin or self intersection intersection
    float t = numerator / denominator;
    if (t < grace) { return -1; }
    Vector3D poi = ray.pointAt(t);
    Triangle a(poi, triangle.v2, triangle.v3, triangle.materialColor);
    Triangle b(poi, triangle.v1, triangle.v3, triangle.materialColor);
    Triangle c(poi, triangle.v1, triangle.v2, triangle.materialColor);
    // Out of triangle intersection
    if (a.area + b.area + c.area - triangle.area > 1e-3) { return -1; }
    // Intersects inside triangle
    return t;
}
