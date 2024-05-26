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
    Tracer(const std::vector<Shape *> &scene_) : scene(scene_) {}
    
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
    
    Vector getRadiance(const Ray &r, int depth) {
        auto result = getIntersection(r);
        Shape *hitObj = result.first;
        if (!hitObj) return Vector();
        
        double U = drand48();
        if (depth > 4) {
            double terminationProbability = hitObj->color.max();  // the Russian roulette is based on the maximum color component
            if (depth > 20 || U > terminationProbability) {
                return Vector();
            }
            // Scale the radiance by the survival probability to maintain energy conservation
            hitObj->color = hitObj->color / terminationProbability;
        }

        // if (depth > 0) {
        //     return Vector();
        // }

        Vector hitPos = r.origin + r.direction * result.second;
        Vector norm = hitObj->getNormal(hitPos);
        if (norm.dot(r.direction) > 0) {
            norm = norm * -1;
        }
        Vector color = hitObj->getColor(hitPos);
        Vector lightSampling;
        if (EMITTER_SAMPLING) {
            for (Shape *light : scene) {
                if (light->emit.max() == 0) {
                    continue;
                }
                Vector lightPos = light->randomPoint();
                Vector lightDirection = (lightPos - hitPos).norm();
                Ray rayToLight = Ray(hitPos, lightDirection);
                auto lightHit = getIntersection(rayToLight);
                if (light == lightHit.first) {
                    double wi = lightDirection.dot(norm);
                    if (wi > 0) {
                        double srad = 1.5;
                        double cos_a_max = sqrt(1-srad*srad/(hitPos - lightPos).dot(hitPos - lightPos));
                        double omega = 2*M_PI*(1-cos_a_max);
                        lightSampling += light->emit * wi * omega * M_1_PI;
                    }
                }
            }
        }
        double angle = 2 * M_PI * drand48();
        double dist_cen = sqrt(drand48());
        Vector u;
        if (fabs(norm.x) > 0.1) {
            u = Vector(0, 1, 0);
        } else {
            u = Vector(1, 0, 0);
        }
        u = u.cross(norm).norm();
        Vector v = norm.cross(u);
        Vector d = (u * cos(angle) * dist_cen + v * sin(angle) * dist_cen + norm * sqrt(1 - dist_cen * dist_cen)).norm();
        Vector reflected = getRadiance(Ray(hitPos, d), depth + 1);
        if (!EMITTER_SAMPLING || depth == 0) {
            return hitObj->emit + color * lightSampling + color * reflected;
        }
        return color * lightSampling + color * reflected;
    }
};

#endif // TRACER_H
