#ifndef SHAPE_H
#define SHAPE_H

#include "vector.h"
#include "ray.h"

struct Shape {
    Vector color, emit;
    Shape(const Vector &color_, const Vector &emit_) : color(color_), emit(emit_) {}
    virtual double intersects(const Ray &r) const { return 0; }
    virtual Vector randomPoint() const { return Vector(); }
    virtual Vector getNormal(const Vector &p) const { return Vector(); }
    virtual Vector getColor(const Vector &p) const { return color; }
    virtual Vector getEmission() const { return emit; }  // Added getEmission method
};

#endif // SHAPE_H