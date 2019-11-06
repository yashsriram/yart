#include <iostream>
#include <stack>
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
#define RECURSIVE_RAY_GRACE 1e-3
#define CAMERA_MEDIUM_REFRACTIVE_INDEX 1
#define CAMERA_MEDIUM_OPACITY 0
#define RECURSIVE_DEPTH 6
#define SOFT_SHADOW_JITTER 0
#define NUM_SHADOW_RAYS_PER_POI 1
#define NUM_DISTRIBUTED_RAYS 10
#define DISTRIBUTED_RAYS_JITTER 5e-2

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
    int minTIndex = indexOfSmallestNonNegativeElement(Ts);

    return {minTIndex, minTIndex < 0 ? -1 : Ts[minTIndex]};
}

// Given point of intersection, unit direction to light source, light and scene
// Calculates if there is a shadow cast on point of intersection by the light source
// By casting a shadow ray from poi in unit direction to light source
float shadowFactor(const Vector3D &poi, const Vector3D &Li, const Light &light, const Scene &scene) {
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
            Vector3D hitPoint = shadowRay.pointAt(shadow_minTIndex_minT.second);
            Vector3D hitVector = hitPoint - poi;
            Vector3D lightVector = light.vector - poi;
            if (hitVector.absSquare() < lightVector.absSquare()) {
                S = 0;
            }
        }
    }
    return S;
}

// Given point of intersection, unit direction to light source, light and scene
// Foreach point of intersection by ray from poi to light source decreases shadow factor
float shadowFactorSubtractive(const Vector3D &poi, const Vector3D &Li, const Light &light, const Scene &scene) {
    float S = 1;
    Ray shadowRay(poi, Li);
    if (light.type == 0) {
        // Directional light => Shadow exists
        // Inspect intersection with all spheres
        for (const auto &sphere : scene.spheres) {
            float t = smallestNonNegativeT(shadowRay, sphere, SHADOW_GRACE);
            if (t > -1) {
                S = S * (1 - sphere.materialColor.opacity);
            }
        }
        // Inspect intersection with all triangles
        for (const auto &triangle : scene.triangles) {
            float t = smallestNonNegativeT(shadowRay, triangle, SHADOW_GRACE);
            if (t > -1) {
                S = S * (1 - triangle.materialColor.opacity);
            }
        }
    } else {
        // Positional light => Check for distance of hit
        // Inspect intersection with all spheres
        for (const auto &sphere : scene.spheres) {
            float t = smallestNonNegativeT(shadowRay, sphere, SHADOW_GRACE);
            if (t > -1) {
                Vector3D hitPoint = shadowRay.pointAt(t);
                Vector3D hitVector = hitPoint - poi;
                Vector3D lightVector = light.vector - poi;
                if (hitVector.absSquare() < lightVector.absSquare()) {
                    S = S * (1 - sphere.materialColor.opacity);
                }
            }
        }
        // Inspect intersection with all triangles
        for (const auto &triangle : scene.triangles) {
            float t = smallestNonNegativeT(shadowRay, triangle, SHADOW_GRACE);
            if (t > -1) {
                Vector3D hitPoint = shadowRay.pointAt(t);
                Vector3D hitVector = hitPoint - poi;
                Vector3D lightVector = light.vector - poi;
                if (hitVector.absSquare() < lightVector.absSquare()) {
                    S = S * (1 - triangle.materialColor.opacity);
                }
            }
        }
    }
    return S;
}

// Given ray, scene and intersecting object and point
// returns appropriate color to fill in the corresponding pixel of output image
Color phongColorForSphere(const Ray &ray, const Scene &scene, const Sphere &sphere, const Vector3D &poi) {
    // Blinn-phong illumination model
    // I = Od * ka + Sum over lights [Si * Ilight (Od * kd * (N.L) + Os * ks * (N.H)^n)]
    // Intersection with a sphere
    MaterialColor color = sphere.materialColor;
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
            S += shadowFactorSubtractive(poi, Lj, light, scene);
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

// Given ray, scene and intersecting object and point
// returns appropriate color to fill in the corresponding pixel of output image
Color phongColorForTriangle(const Ray &ray, const Scene &scene, const Triangle &triangle, const Vector3D &poi) {
    // Blinn-phong illumination model
    // I = Od * ka + Sum over lights [Si * Ilight (Od * kd * (N.L) + Os * ks * (N.H)^n)]
    // Intersection with an Triangle
    MaterialColor color = triangle.materialColor;
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
            S += shadowFactorSubtractive(poi, Lj, light, scene);
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

Color traceRayRecursive(const Ray &ray, const Scene &scene, const float grace,
                        const int depth, stack<float> refractiveIndices, stack<float> opacities) {
    pair<int, float> minTIndex_minT = traceRay(ray, scene, grace);
    int objIndex = minTIndex_minT.first;
    float paramT = minTIndex_minT.second;
    int noSpheres = scene.spheres.size();

    // no intersection with anything
    if (objIndex < 0) {
        // return variant of bg color
        return scene.bgColor;
    }

    Color reflectedColor;
    Color transmittedColor;
    Color tirColor;
    Vector3D poi = ray.pointAt(paramT);
    // ray intersects an object and can still recurse
    if (depth > 0) {
        const Vector3D I = (ray.origin - poi).unit();
        const stack<float> refractiveIndicesCopy = refractiveIndices;
        const float prevRI = refractiveIndicesCopy.top();
        const stack<float> opacitiesCopy = opacities;

        // next object RI, opacity and normal at POI
        float nextRI = 0;
        float nextOpacity = -1;
        Vector3D N;
        if (objIndex < noSpheres) {
            Sphere sphere = scene.spheres[objIndex];
            nextRI = sphere.materialColor.refractiveIndex;
            nextOpacity = sphere.materialColor.opacity;
            N = (poi - sphere.center).unit();
        } else {
            Triangle triangle = scene.triangles[objIndex - noSpheres];
            nextRI = triangle.materialColor.refractiveIndex;
            nextOpacity = triangle.materialColor.opacity;
            N = triangle.surfaceNormal.unit();
        }

        // Entering or exiting object
        if (N.dot(I) < 0) {
            // Case refers to ray exiting object
            // Correcting normal
            N = N * -1;
            // Removing top of refractive indices and opacities stack
            if (refractiveIndices.size() > 1) {
                refractiveIndices.pop();
            }
            if (opacities.size() > 1) {
                opacities.pop();
            }
            // Correcting nextRI and nextOpacity
            nextRI = refractiveIndices.top();
            nextOpacity = opacities.top();
        } else {
            refractiveIndices.push(nextRI);
            opacities.push(nextOpacity);
        }

        const float cosThetaI = N.dot(I);
        const float F0 = pow((nextRI - prevRI) / (nextRI + prevRI), 2);

        // Reflection
        const float Fr = F0 + (1 - F0) * pow((1 - cosThetaI), 5);
        const Vector3D R = (N * 2 * cosThetaI - I).unit();
        const Ray reflectedRay(poi, R);
        // the nextRI = prevRI as ray doesn't leave medium
        reflectedColor = traceRayRecursive(reflectedRay, scene, grace, depth - 1, refractiveIndicesCopy, opacitiesCopy);
        reflectedColor = reflectedColor * Fr;

        // Refraction
        const float underSqrtTerm = 1 - (pow((prevRI / nextRI), 2) * (1 - pow(cosThetaI, 2)));
        if (underSqrtTerm >= 0) {
            // normal refraction
            const Vector3D T = (N * -sqrt(underSqrtTerm) + (N * cosThetaI - I) * (prevRI / nextRI)).unit();
            const Ray transmittedRay(poi, T);
            transmittedColor = traceRayRecursive(transmittedRay, scene, grace, depth - 1, refractiveIndices, opacities);
            transmittedColor = transmittedColor * (1 - Fr) * (1 - nextOpacity);
        } else {
            // total internal reflection
            // this can be optimized as this tracing is same as the reflected ray tracing in the same recursion depth
            tirColor = traceRayRecursive(reflectedRay, scene, grace, depth - 1, refractiveIndicesCopy, opacitiesCopy);
            tirColor = tirColor * (1 - Fr);
        }
    }

    // phongColor = ambient + diffuse + specular + shadows
    Color phongColor;
    if (objIndex < noSpheres) {
        Sphere sphere = scene.spheres[objIndex];
        phongColor = phongColorForSphere(ray, scene, sphere, poi);
    } else {
        Triangle triangle = scene.triangles[objIndex - noSpheres];
        phongColor = phongColorForTriangle(ray, scene, triangle, poi);
    }
    return phongColor + reflectedColor + transmittedColor + tirColor;
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
    float viewingWindowWidth = scene.imWidth;
    float viewingWindowHeight = scene.imHeight;
    if (scene.viewingDistance > 0) {
        viewingWindowWidth = (scene.viewingDistance / d) * scene.imWidth;
        viewingWindowHeight = (scene.viewingDistance / d) * scene.imHeight;
        d = scene.viewingDistance;
    }

    Vector3D imageCenter = scene.eye + scene.viewDir.unit() * d;
    Vector3D ul = imageCenter - u * (viewingWindowWidth / 2) + v * (viewingWindowHeight / 2);
    Vector3D ur = imageCenter + u * (viewingWindowWidth / 2) + v * (viewingWindowHeight / 2);
    Vector3D ll = imageCenter - u * (viewingWindowWidth / 2) - v * (viewingWindowHeight / 2);
    Vector3D lr = imageCenter + u * (viewingWindowWidth / 2) - v * (viewingWindowHeight / 2);

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
            // trace this ray in the scene recursively to produce a color for the pixel
            stack<float> refractiveIndices;
            refractiveIndices.push(CAMERA_MEDIUM_REFRACTIVE_INDEX);
            stack<float> opacities;
            opacities.push(CAMERA_MEDIUM_OPACITY);

            if (scene.viewingDistance > 0) {
                // Control NUM_DISTRIBUTED_RAYS, DISTRIBUTED_RAYS_JITTER to change distributed ray tracing effects
                // Set NUM_DISTRIBUTED_RAYS = 1, DISTRIBUTED_RAYS_JITTER = 0 for disabling distributed ray tracing
                for (int distributed_i = 0; distributed_i < NUM_DISTRIBUTED_RAYS; distributed_i++) {
                    // Create ray and add jitter to ray origin
                    Ray ray;
                    if (scene.isParallelProjection) {
                        // ray from pixel projection on eye plane in direction of normal to image plane
                        ray = Ray(pixelCoordinate - scene.viewDir.unit() * d +
                                  Vector3D(getRand(), getRand(), getRand()).unit() * DISTRIBUTED_RAYS_JITTER,
                                  scene.viewDir);
                    } else {
                        // ray from eye to that pixel
                        Vector3D origin =
                                scene.eye + Vector3D(getRand(), getRand(), getRand()).unit() * DISTRIBUTED_RAYS_JITTER;
                        ray = Ray(origin, (pixelCoordinate - origin).unit());
                    }
                    Color color = traceRayRecursive(ray, scene,
                                                    RECURSIVE_RAY_GRACE, RECURSIVE_DEPTH,
                                                    refractiveIndices,
                                                    opacities);
                    // Keep track of color
                    colors[i][j] = colors[i][j] + color;
                }
                colors[i][j] = colors[i][j] * (1.0 / NUM_DISTRIBUTED_RAYS);
            } else {
                // Create ray and add jitter to ray origin
                Ray ray;
                if (scene.isParallelProjection) {
                    // ray from pixel projection on eye plane in direction of normal to image plane
                    ray = Ray(pixelCoordinate - scene.viewDir.unit() * d, scene.viewDir);
                } else {
                    // ray from eye to that pixel
                    Vector3D origin = scene.eye;
                    ray = Ray(scene.eye, (pixelCoordinate - origin).unit());
                }
                Color color = traceRayRecursive(ray, scene,
                                                RECURSIVE_RAY_GRACE, RECURSIVE_DEPTH,
                                                refractiveIndices,
                                                opacities);
                // Keep track of color
                colors[i][j] = color;
            }

            // Show progress
            if (i == 0) {
                printf("Rendering: %d%% complete\r", (int) ((float) (j + 1) * 100 / scene.imHeight));
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
