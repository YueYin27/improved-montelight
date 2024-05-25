#ifndef SPHERE_H
#define SPHERE_H
#define EPSILON 0.001f

#include "shape.h"
#include <cmath>

struct Sphere : Shape {
    Vector center;
    double radius;
    Sphere(const Vector center_, double radius_, const Vector color_, const Vector emit_) :
        Shape(color_, emit_), center(center_), radius(radius_) {}
    
    double intersects(const Ray &r) const override {
        Vector offset = r.origin - center;
        double a = r.direction.dot(r.direction);
        double b = 2 * offset.dot(r.direction);
        double c = offset.dot(offset) - radius * radius;
        double disc = b * b - 4 * a * c;
        if (disc < 0) {
            return 0;
        }
        disc = sqrt(disc);
        double t = -b - disc;
        if (t > EPSILON) {
            return t / 2;
        }
        t = -b + disc;
        if (t > EPSILON) {
            return t / 2;
        }
        return 0;
    }
    
    Vector randomPoint() const override {
        double theta = drand48() * M_PI;
        double phi = drand48() * 2 * M_PI;
        double dxr = radius * sin(theta) * cos(phi);
        double dyr = radius * sin(theta) * sin(phi);
        double dzr = radius * cos(theta);
        return Vector(center.x + dxr, center.y + dyr, center.z + dzr);
    }
    
    Vector getNormal(const Vector &p) const override {
        return (p - center) / radius;
    }
};

#endif // SPHERE_H
