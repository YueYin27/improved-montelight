#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include "shape.h"

struct Checkerboard : Shape {
    Vector color1, color2;
    double size;

    Checkerboard(const Vector &color1_, const Vector &color2_, double size_, const Vector &emit_) :
        Shape(Vector(), emit_), color1(color1_), color2(color2_), size(size_) {}

    double intersects(const Ray &r) const override {
        // Check for intersection with the xz-plane (floor)
        if (fabs(r.direction.y) < EPSILON) return 0;
        double t = -r.origin.y / r.direction.y;
        if (t < 0) return 0;
        return t;
    }

    Vector getNormal(const Vector &p) const override {
        return Vector(0, 1, 0); // Normal pointing up
    }

    Vector getColor(const Vector &p) const override {
        // Calculate the checkerboard pattern
        int x = floor(p.x / size);
        int z = floor(p.z / size);
        if ((x + z) % 2 == 0) {
            return color1;
        } else {
            return color2;
        }
    }

    Vector randomPoint() const override {
        return Vector(); // Not used for the floor
    }
};

#endif // CHECKERBOARD_H
