#ifndef CUBE_H
#define CUBE_H

#include "shape.h"
#include <cmath>
#include <algorithm> // Include this for std::swap

#define EPSILON 0.001f // Define EPSILON here

struct Cube : Shape {
    Vector min, max, center;
    double angle; // Rotation angle in radians

    Cube(const Vector &min_, const Vector &max_, const Vector &color_, const Vector &emit_, double angle_) :
        Shape(color_, emit_), min(min_), max(max_), center((min_ + max_) / 2), angle(angle_) {}

    // Function to rotate a point around the center of the cube
    Vector rotatePoint(const Vector &p) const {
        Vector translated = p - center;
        double sinAngle = sin(angle);
        double cosAngle = cos(angle);

        double x = translated.x * cosAngle - translated.z * sinAngle;
        double z = translated.x * sinAngle + translated.z * cosAngle;

        return Vector(x, translated.y, z) + center;
    }

    double intersects(const Ray &r) const override {
        // Rotate the ray to match the cube's orientation
        Ray rotatedRay(rotatePoint(r.origin), rotatePoint(r.direction + r.origin) - rotatePoint(r.origin));

        double tmin = (min.x - rotatedRay.origin.x) / rotatedRay.direction.x;
        double tmax = (max.x - rotatedRay.origin.x) / rotatedRay.direction.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        double tymin = (min.y - rotatedRay.origin.y) / rotatedRay.direction.y;
        double tymax = (max.y - rotatedRay.origin.y) / rotatedRay.direction.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax)) return 0;
        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;

        double tzmin = (min.z - rotatedRay.origin.z) / rotatedRay.direction.z;
        double tzmax = (max.z - rotatedRay.origin.z) / rotatedRay.direction.z;
        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax)) return 0;
        if (tzmin > tmin) tmin = tzmin;
        if (tzmax < tmax) tmax = tzmax;

        return tmin > 0 ? tmin : tmax;
    }

    Vector getNormal(const Vector &p) const override {
        Vector rotatedP = rotatePoint(p);
        if (fabs(rotatedP.x - min.x) < EPSILON) return Vector(-1, 0, 0);
        if (fabs(rotatedP.x - max.x) < EPSILON) return Vector(1, 0, 0);
        if (fabs(rotatedP.y - min.y) < EPSILON) return Vector(0, -1, 0);
        if (fabs(rotatedP.y - max.y) < EPSILON) return Vector(0, 1, 0);
        if (fabs(rotatedP.z - min.z) < EPSILON) return Vector(0, 0, -1);
        if (fabs(rotatedP.z - max.z) < EPSILON) return Vector(0, 0, 1);
        return Vector();
    }

    Vector randomPoint() const override {
        double x = min.x + drand48() * (max.x - min.x);
        double y = min.y + drand48() * (max.y - min.y);
        double z = min.z + drand48() * (max.z - min.z);
        return rotatePoint(Vector(x, y, z));
    }
};

#endif // CUBE_H
