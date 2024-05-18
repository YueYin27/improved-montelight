#ifndef RAY_H
#define RAY_H

#include "vector.h"

struct Ray {
    Vector origin, direction;
    Ray(const Vector &o_, const Vector &d_) : origin(o_), direction(d_) {}
};

#endif // RAY_H
