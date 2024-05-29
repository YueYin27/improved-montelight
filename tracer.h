#ifndef TRACER_H
#define TRACER_H
#include <vector>
#include <cmath>
#include "shapes.h"
#include "ray.h"
#include "vector.h"

extern bool EMITTER_SAMPLING;

struct Tracer {
    std::vector<Shape *> scene;
    Vector cameraPos;  // Add this line
    Tracer(const std::vector<Shape *> &scene_, const Vector &cameraPos_) 
        : scene(scene_), cameraPos(cameraPos_) {}
    // Tracer(const std::vector<Shape *> &scene_) : scene(scene_) {}
    std::pair<Shape *, double> getIntersection(const Ray &r) const {
        Shape *hitObj = nullptr;
        double closest = 1e20f;
        for (Shape *obj : scene) {
            double distToHit = obj->intersects(r);
            if (distToHit > 0 && distToHit < closest) {
                hitObj = obj;
                closest = distToHit;
            }
        }
        return std::make_pair(hitObj, closest);
    }

    Vector refract(const Vector &I, const Vector &N, const float &ior) const { 
        float cosi = std::max(-1.0, std::min(1.0, I.dot(N)));
        float etai = 1, etat = ior;
        Vector n = N;
        if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n = -N; }
        float eta = etai / etat;
        float k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? 0 : I * eta + n * (eta * cosi - sqrtf(k));
    }

    Vector reflect (const Vector &I, const Vector &N) {
        return I - N * 2 * I.dot(N);
    }

    float fresnel(const Vector &I, const Vector &N, const float &ior)
    {
        float cosi = std::max(-1.0, std::min(1.0, I.dot(N)));
        float etai = 1, etat = ior;
        if (cosi > 0) {  std::swap(etai, etat); }
        // Compute sini using Snell's law
        float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
        // Total internal reflection
        if (sint >= 1) {
            return 1;
        }
        else {
            float cost = sqrtf(std::max(0.f, 1 - sint * sint));
            cosi = fabsf(cosi);
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            return (Rs * Rs + Rp * Rp) / 2;
        }
        // As a consequence of the conservation of energy, transmittance is given by:
        // kt = 1 - kr;
    }

    Vector getRadiance(const Ray &r, int depth) {
        auto result = getIntersection(r);
        Shape *hitObj = result.first;
        if (!hitObj) return Vector();  // Return black if no intersection
        if (hitObj->emit.max() > 0) return hitObj->emit;  // Return the emission of the object if it is an emitter

        double U = drand48();
        double terminationProbability = hitObj->color.max();  // the Russian roulette is based on the maximum color component
        if (depth > 4) {
            if (depth > 10 || U > terminationProbability) {
                return Vector();
            }
            // Scale the radiance by the survival probability to maintain energy conservation
            hitObj->color = hitObj->color / terminationProbability;
        }

        // if (depth > 0) return Vector();

        Vector hitPos = r.origin + r.direction * result.second;
        Vector normal = hitObj->getNormal(hitPos);
        if (normal.dot(r.direction) > 0) {
            normal = normal * -1;
        }
        Vector color = hitObj->getColor(hitPos);
        Vector lightSampling;
        if (hitObj->material == MIRROR) {
            // Calculate reflection direction
            Vector reflectionDirection = (reflect(r.direction, normal)).normalize();
            // Get radiance along the reflection direction
            return hitObj->emit + color * getRadiance(Ray(hitPos, reflectionDirection), depth + 1);
        }

        else if (hitObj->material == GLASS) {
            
            // set a fixed IOR for all glass
            double ior = 2;
            
            // Calculate reflection direction and refraction direction
            Vector reflectionDirection = (reflect(r.direction, normal)).normalize();
            Vector refractionDirection = (refract(r.direction, normal, ior)).normalize();

            // Compute reflection and refraction ray origins (offset the ray slightly off the surface along the surface normal)
            Vector reflectionOrigin = (reflectionDirection.dot(normal) < 0) ? hitPos - normal * EPSILON : hitPos + normal * EPSILON;
            Vector refractionOrigin = (refractionDirection.dot(normal) < 0) ? hitPos - normal * EPSILON : hitPos + normal * EPSILON;

            // Compute reflection and refraction colors
            Vector reflectionColor = getRadiance(Ray(reflectionOrigin, reflectionDirection), depth + 1);
            Vector refractionColor = getRadiance(Ray(refractionOrigin, refractionDirection), depth + 1);

            // Use Fresnel equation to compute the ratio of reflection and refraction
            float kr = fresnel(r.direction, normal, ior);
            return hitObj->emit + color * (reflectionColor * kr + refractionColor * (1 - kr));
        }

        else {
            if (EMITTER_SAMPLING) {
                for (Shape *light : scene) {
                    if (light->emit.max() == 0) continue;  // Skip non-emissive objects

                    Vector lightPos = light->randomPoint();
                    Vector lightDirection = (lightPos - hitPos).normalize();
                    Ray rayToLight = Ray(hitPos, lightDirection);
                    auto lightHit = getIntersection(rayToLight);
                    if (light == lightHit.first) {
                        double wi = lightDirection.dot(normal);
                        if (wi > 0) {
                            double srad = 1.5;
                            double cos_a_max = sqrt(1 - srad * srad / (hitPos - lightPos).dot(hitPos - lightPos));
                            double omega = 2 * M_PI * (1 - cos_a_max);
                            lightSampling += light->emit * wi * omega * M_1_PI;
                        }
                    }
                }
            }

            double angle = 2 * M_PI * drand48();
            double dist_cen = sqrt(drand48());
            Vector u;
            if (fabs(normal.x) > 0.1) {
                u = Vector(0, 1, 0);
            } else {
                u = Vector(1, 0, 0);
            }
            u = u.cross(normal).normalize();
            Vector v = normal.cross(u);
            Vector d = (u * cos(angle) * dist_cen + v * sin(angle) * dist_cen + normal * sqrt(1 - dist_cen * dist_cen)).normalize();
            Vector reflected = getRadiance(Ray(hitPos, d), depth + 1);
            if (!EMITTER_SAMPLING || depth == 0) {
                return hitObj->emit + color * lightSampling + color * reflected;
            }
            return color * lightSampling + color * reflected;
        }
    }

};

#endif // TRACER_H
