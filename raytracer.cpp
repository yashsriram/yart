#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <utility>
#include "include/vector3d.hpp"
#include "include/color.hpp"
#include "include/ray.hpp"
#include "include/sphere.hpp"
#include "include/ellipsoid.hpp"
#include "include/scene.hpp"
#include "include/intersections.hpp"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#define SHADOW_GRACE 1e-4
#define SOFT_SHADOW_JITTER 0
#define SOFT_SHADOW_NO_SAMPLES 1

// Returns index of smallest non-negative number from vector
// If all are negative then returns -1
int smallestNonNegativeIndex(const vector<float> &vector) {
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

pair<int, float> traceRay(const Ray &ray, const Scene &scene, float grace = 0) {
    vector<float> Ts;
    // Inspect intersection with all spheres
    for (const auto &sphere : scene.spheres) {
        float t = smallestPositiveT(ray, sphere, grace);
        Ts.push_back(t);
    }
    // Inspect intersection with all ellipsoids
    for (const auto &ellipsoid : scene.ellipsoids) {
        float t = smallestPositiveT(ray, ellipsoid, grace);
        Ts.push_back(t);
    }
    // Find smallest non-negative t
    int minTIndex = smallestNonNegativeIndex(Ts);

    return pair<int, float>(minTIndex, minTIndex < 0 ? -1 : Ts[minTIndex]);
}

float calculateShadowFactor(const Vector3D &poi, const Vector3D &Li, const Light &light, const Scene &scene) {
    float S = 1;
    Ray shadowRay(poi, Li);
    pair<int, float> shadow_minTIndex_minT = traceRay(shadowRay, scene, SHADOW_GRACE);
    // Shadow ray hit something
    if (shadow_minTIndex_minT.first >= 0) {
        if (light.type == 0) {
            // Directional light => Shadow exists
            S = 0;
        } else {
            // Positional light => Check for distance of hit
            Vector3D hitPoint = shadowRay.getPoint(shadow_minTIndex_minT.second);
            Vector3D hitVector = hitPoint - poi;
            Vector3D lightVector = light.vector - poi;
            if (hitVector.absSquare() < lightVector.absSquare()) {
                S = 0;
            }
        }
    }
    return S;
}

string calculateColor(const Ray &ray, const Scene &scene, int minTIndex, float minT) {
    int noSpheres = scene.spheres.size();
    // No intersection with anything
    if (minTIndex < 0) {
        // Return variant of bg color
        return (scene.bgColor * ray.direction.dot(scene.viewDir.unit())).to8BitScale();
    } else {
        // Intersection with a sphere
        if (minTIndex < noSpheres) {
            Sphere sphere = scene.spheres[minTIndex];
            MaterialColor color = sphere.materialColor;
            Vector3D poi = ray.getPoint(minT);
            Vector3D N = (poi - sphere.center).unit();
            Vector3D V = (scene.eye - poi).unit();
            // First term of blinn-phong model
            Color phongColor = color.diffusion * color.ka;

            for (auto &light: scene.lights) {
                // Shadow factor determination
                float S = 0;
                for (int j = 0; j < SOFT_SHADOW_NO_SAMPLES; j++) {
                    Vector3D Lj = light.poiToLightUnitVector(poi, SOFT_SHADOW_JITTER);
                    S += calculateShadowFactor(poi, Lj, light, scene);
                }
                S = S / SOFT_SHADOW_NO_SAMPLES;
                // Second and third terms of blinn-phong model
                Vector3D Li = light.poiToLightUnitVector(poi);
                Vector3D Hi = (Li + V).unit();
                Color secondTerm = color.diffusion * color.kd * max(0.0, (double) N.dot(Li));
                Color thirdTerm = color.specular * color.ks * pow(max(0.0, (double) N.dot(Hi)), color.n);
                Color weightedTerm = (secondTerm + thirdTerm) * light.color * S;
                phongColor = phongColor + weightedTerm;
            }

            phongColor.clamp();
            return phongColor.to8BitScale();
        }
            // Intersection with an ellipsoid
        else {
            return scene.ellipsoids[minTIndex - noSpheres].materialColor.diffusion.to8BitScale();
        }
    }
}

int main(int argc, char *argv[]) {
    // Initializing random seed
    srand(42);

    // Restricting number of commandline arguments
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <inputfile>" << endl;
        exit(-1);
    }
    string filename(argv[1]);

    // Read scene description from input file
    Scene scene(filename);
    if (!scene.parse()) {
        return -1;
    };

    cout << scene;

    // Preliminary calculations
    Vector3D u = scene.viewDir.cross(scene.upDir);
    Vector3D v = u.cross(scene.viewDir);
    Vector3D n = u.cross(v);

    float d = scene.imHeight / (2 * tan(scene.vFovDeg * M_PI / 360));

    Vector3D imageCenter = scene.eye + scene.viewDir.unit() * d;
    Vector3D ul = imageCenter - u * (scene.imWidth / 2) + v * (scene.imHeight / 2);
    Vector3D ur = imageCenter + u * (scene.imWidth / 2) + v * (scene.imHeight / 2);
    Vector3D ll = imageCenter - u * (scene.imWidth / 2) - v * (scene.imHeight / 2);
    Vector3D lr = imageCenter + u * (scene.imWidth / 2) - v * (scene.imHeight / 2);

    Vector3D delWidth = (ur - ul) * (1 / (scene.imWidth - 1));
    Vector3D delHeight = (ll - ul) * (1 / (scene.imHeight - 1));

    // Initialize pixel array for output image
    vector<vector<string> > colors;
    for (int i = 0; i < scene.imWidth; i++) {
        vector<string> col;
        col.resize(scene.imHeight);
        colors.push_back(col);
    }

    // Ray tracing per pixel
    for (int j = 0; j < scene.imHeight; j++) {
        for (int i = 0; i < scene.imWidth; i++) {
            // (i, j) pixel coordinate
            Vector3D pixelCoordinate = ul + delWidth * i + delHeight * j;
            // ray from eye to that pixel
            Ray ray(scene.eye, (pixelCoordinate - scene.eye).unit());
            // trace this ray in the scene to produce a color for the pixel
            pair<int, float> minTIndex_minT = traceRay(ray, scene);
            string pixelValue = calculateColor(ray, scene, minTIndex_minT.first, minTIndex_minT.second);
            // Keep track of color
            colors[i][j] = pixelValue;
        }
    }


    // Write the final image to an output file
    string outputfileString(filename);
    int len = outputfileString.size();
    outputfileString.replace(len - 3, 3, "ppm");

    // Creating PPM image
    ofstream outputfile;
    outputfile.open(outputfileString);

    // Filling in the header
    outputfile << "P3" << endl << "# image autogenerated using a simple ray tracer" << endl;
    outputfile << scene.imWidth << " " << scene.imHeight << endl;
    outputfile << 255 << endl;

    // Filling in the body
    for (int j = 0; j < scene.imHeight; ++j) {
        for (int i = 0; i < scene.imWidth; ++i) {
            // Write the R G B values of a pixel in seperate line
            outputfile << colors[i][j] << endl;
        }
    }

    // Closing the output stream
    outputfile.close();

    return 0;
}
