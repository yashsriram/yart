#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <utility>
#include "include/vector3d.hpp"
#include "include/color.hpp"
#include "include/ray.hpp"
#include "include/sphere.hpp"
#include "include/triangle.hpp"
#include "include/scene.hpp"
#include "include/intersections.hpp"
#include "include/texture.hpp"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#define SHADOW_GRACE 1e-4
#define SOFT_SHADOW_JITTER 0
#define NUM_SHADOW_RAYS_PER_POI 1

// Returns index of smallest non-negative number from vector
// If all are negative then returns -1
int indexOfSmallestNonNegative(const vector<float> &vector) {
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

// Returns global index of object the ray first hits (in front of the origin) and corresponding T parameter of the hit
// If ray does not hit any object both index and T parameter are returned -1
pair<int, float> traceRay(const Ray &ray, const Scene &scene, float grace = 0) {
    vector<float> Ts;
    // Inspect intersection with all spheres
    for (const auto &sphere : scene.spheres) {
        float t = smallestNonNegativeT(ray, sphere, grace);
        Ts.push_back(t);
    }
    // Inspect intersection with all triangles
    for (const auto &triangle : scene.triangles) {
        float t = smallestNonNegativeT(ray, triangle, grace);
        Ts.push_back(t);
    }
    // Find smallest non-negative t
    int minTIndex = indexOfSmallestNonNegative(Ts);

    return {minTIndex, minTIndex < 0 ? -1 : Ts[minTIndex]};
}

// Given point of intersection, unit direction to light source, light and scene
// Calculates if there is a shadow cast on point of intersection by the light source
// By casting a shadow ray from poi in unit direction to light source
int isUnderShadow(const Vector3D &poi, const Vector3D &Li, const Light &light, const Scene &scene) {
    int S = 1;
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

// Given ray, scene and intersecting object and point returns appropriate color to fill in the corresponding pixel of output image
Color calculateColor(const Ray &ray, const Scene &scene, int minTIndex, float minT) {
    int noSpheres = scene.spheres.size();
    // No intersection with anything
    if (minTIndex < 0) {
        // Return variant of bg color
        return scene.bgColor;
    } else {
        // Blinn-phong illumination model
        // I = Od * ka + Sum over lights [Si * Ilight (Od * kd * (N.L) + Os * ks * (N.H)^n)]
        if (minTIndex < noSpheres) {
            // Intersection with a sphere
            Sphere sphere = scene.spheres[minTIndex];
            MaterialColor color = sphere.materialColor;
            Vector3D poi = ray.getPoint(minT);
            Vector3D N = (poi - sphere.center).unit();
            Vector3D V = (scene.eye - poi).unit();
            Color diffusion;
            // Diffusion color based on texture
            if (sphere.renderType == TEXTURE_LESS) {
                diffusion = color.diffusion;
            } else {
                float phi = acos(N.y);
                float theta = atan2(N.x, N.z);
                TextureCoordinates textureCoordinates((theta + M_PI) / (2 * M_PI), phi / M_PI);
                diffusion = scene.textures[sphere.textureIndex].colorAt(textureCoordinates);
            }
            // First term of blinn-phong model
            Color phongColor = diffusion * color.ka;
            for (auto &light: scene.lights) {
                // Shadow factor determination
                float S = 0;
                for (int j = 0; j < NUM_SHADOW_RAYS_PER_POI; j++) {
                    Vector3D Lj = light.poiToLightUnitVector(poi, SOFT_SHADOW_JITTER);
                    S += (float) isUnderShadow(poi, Lj, light, scene);
                }
                S = S / NUM_SHADOW_RAYS_PER_POI;

                // Second and third terms of blinn-phong model
                Vector3D Li = light.poiToLightUnitVector(poi);
                Vector3D Hi = (Li + V).unit();
                Color secondTerm = diffusion * color.kd * max(0.0, (double) N.dot(Li));
                Color thirdTerm = color.specular * color.ks * pow(max(0.0, (double) N.dot(Hi)), color.n);
                Color weightedTerm = (secondTerm + thirdTerm) * light.color * S;
                phongColor = phongColor + weightedTerm;
            }

            return phongColor;
        } else {
            // Intersection with an Triangle
            Triangle triangle = scene.triangles[minTIndex - noSpheres];
            MaterialColor color = triangle.materialColor;
            Vector3D poi = ray.getPoint(minT);
            Vector3D V = (scene.eye - poi).unit();
            Vector3D N;
            Color diffusion;
            // Diffusion color and normal based on texture and smoothness
            if (triangle.renderType == FLAT_TEXTURE_LESS) {
                N = triangle.surfaceNormal.unit();
                diffusion = color.diffusion;
            } else if (triangle.renderType == FLAT_TEXTURED) {
                N = triangle.surfaceNormal.unit();
                TextureCoordinates textureCoordinates = triangle.getInterpolatedTextureCoordinates(poi);
                diffusion = scene.textures[triangle.textureIndex].colorAt(textureCoordinates);
            } else if (triangle.renderType == SMOOTH_TEXTURE_LESS) {
                N = triangle.getInterpolatedNormal(poi);
                diffusion = color.diffusion;
            } else if (triangle.renderType == SMOOTH_TEXTURED) {
                N = triangle.getInterpolatedNormal(poi);
                TextureCoordinates textureCoordinates = triangle.getInterpolatedTextureCoordinates(poi);
                diffusion = scene.textures[triangle.textureIndex].colorAt(textureCoordinates);
            }
            // First term of blinn-phong model
            Color phongColor = diffusion * color.ka;
            for (auto &light: scene.lights) {
                // Shadow factor determination
                float S = 0;
                for (int j = 0; j < NUM_SHADOW_RAYS_PER_POI; j++) {
                    Vector3D Lj = light.poiToLightUnitVector(poi, SOFT_SHADOW_JITTER);
                    S += (float) isUnderShadow(poi, Lj, light, scene);
                }
                S = S / NUM_SHADOW_RAYS_PER_POI;

                // Second and third terms of blinn-phong model
                Vector3D Li = light.poiToLightUnitVector(poi);
                Vector3D Hi = (Li + V).unit();
                Color secondTerm = diffusion * color.kd * max(0.0, (double) N.dot(Li));
                Color thirdTerm = color.specular * color.ks * pow(max(0.0, (double) N.dot(Hi)), color.n);
                Color weightedTerm = (secondTerm + thirdTerm) * light.color * S;
                phongColor = phongColor + weightedTerm;
            }

            return phongColor;
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

    // Read scene description from input file
    string filename(argv[1]);
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
    Vector3D ul = imageCenter - u * ((float) scene.imWidth / 2) + v * ((float) scene.imHeight / 2);
    Vector3D ur = imageCenter + u * ((float) scene.imWidth / 2) + v * ((float) scene.imHeight / 2);
    Vector3D ll = imageCenter - u * ((float) scene.imWidth / 2) - v * ((float) scene.imHeight / 2);
    Vector3D lr = imageCenter + u * ((float) scene.imWidth / 2) - v * ((float) scene.imHeight / 2);

    Vector3D delWidth = (ur - ul) * (1 / ((float) scene.imWidth - 1));
    Vector3D delHeight = (ll - ul) * (1 / ((float) scene.imHeight - 1));

    // Initialize pixel array for output image
    vector<vector<Color> > colors;
    for (int i = 0; i < scene.imWidth; i++) {
        vector<Color> col;
        col.resize(scene.imHeight);
        colors.push_back(col);
    }

    // Ray tracing per pixel
    for (int j = 0; j < scene.imHeight; j++) {
        for (int i = 0; i < scene.imWidth; i++) {
            // (i, j) pixel coordinate
            Vector3D pixelCoordinate = ul + delWidth * i + delHeight * j;
            if (scene.isParallelProjection) {
                // ray from pixel projection on eye plane in direction of normal to image plane
                Ray ray(pixelCoordinate - scene.viewDir.unit() * d, scene.viewDir);
                // trace this ray in the scene to produce a color for the pixel
                pair<int, float> minTIndex_minT = traceRay(ray, scene);
                Color color = calculateColor(ray, scene, minTIndex_minT.first, minTIndex_minT.second);
                // Keep track of color
                colors[i][j] = color;
            } else {
                // ray from eye to that pixel
                Ray ray(scene.eye, (pixelCoordinate - scene.eye).unit());
                // trace this ray in the scene to produce a color for the pixel
                pair<int, float> minTIndex_minT = traceRay(ray, scene);
                Color color = calculateColor(ray, scene, minTIndex_minT.first, minTIndex_minT.second);
                // Keep track of color
                colors[i][j] = color;
            }
        }
    }


    // Write the final image to an output file
    string outputFileString(filename);
    int len = outputFileString.size();
    outputFileString.replace(len - 3, 3, "ppm");

    // Creating PPM image
    ofstream outputFile;
    outputFile.open(outputFileString);

    // Filling in the header
    outputFile << "P3" << endl << "# image autogenerated using a simple ray tracer" << endl;
    outputFile << scene.imWidth << " " << scene.imHeight << endl;
    outputFile << 255 << endl;

    // Filling in the body
    for (int j = 0; j < scene.imHeight; ++j) {
        for (int i = 0; i < scene.imWidth; ++i) {
            // Write the R G B values of a pixel in seperate line
            outputFile << colors[i][j].to8BitScale() << endl;
        }
    }

    // Closing the output stream
    outputFile.close();

    return 0;
}
