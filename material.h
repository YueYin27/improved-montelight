#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector.h"

enum MaterialType { DIFFUSE, REFLECTIVE, REFRACTIVE };

struct Material {
    MaterialType type;
    Vector color;
    Vector emission;
    double ior; // Index of refraction for refractive materials

    Material(MaterialType type_, const Vector &color_, const Vector &emission_ = Vector(), double ior_ = 1.0)
        : type(type_), color(color_), emission(emission_), ior(ior_) {}
};

#endif // MATERIAL_H
