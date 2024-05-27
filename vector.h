#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

struct Vector {
    double x, y, z;
    Vector(const Vector &o) : x(o.x), y(o.y), z(o.z) {}
    Vector(double x_=0, double y_=0, double z_=0) : x(x_), y(y_), z(z_) {}
    inline Vector operator+(const Vector &o) const { return Vector(x + o.x, y + o.y, z + o.z); }
    inline Vector &operator+=(const Vector &rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    inline Vector operator-(const Vector &o) const { return Vector(x - o.x, y - o.y, z - o.z); }
    inline Vector operator-() const { return Vector(-x, -y, -z); }  // Unary minus operator
    inline Vector operator*(const Vector &o) const { return Vector(x * o.x, y * o.y, z * o.z); }
    inline Vector operator/(double o) const { return Vector(x / o, y / o, z / o); }
    inline Vector operator*(double o) const { return Vector(x * o, y * o, z * o); }
    inline double dot(const Vector &o) const { return x * o.x + y * o.y + z * o.z; }
    inline Vector normalize() const { return *this * (1 / sqrt(x * x + y * y + z * z)); } // Marked as const
    inline Vector cross(const Vector &o) const { return Vector(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x); }
    inline double min() const { return fmin(x, fmin(y, z)); } // Marked as const
    inline double max() const { return fmax(x, fmax(y, z)); } // Marked as const
    inline Vector &abs() { x = fabs(x); y = fabs(y); z = fabs(z); return *this; }
    inline Vector &clamp() {
        auto clampDouble = [](double x) {
            if (x < 0) return 0.0;
            if (x > 1) return 1.0;
            return x;
        };
        x = clampDouble(x); y = clampDouble(y); z = clampDouble(z);
        return *this;
    }
};

// Overloaded operator for multiplying a float with a Vector
inline Vector operator*(double scalar, const Vector &vec) {
    return Vector(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

#endif // VECTOR_H
