#ifndef SHAPES_H
#define SHAPES_H

#include <cmath>
#include <algorithm> // Include this for std::swap
#include "vector.h"
#include "ray.h"

#define EPSILON 0.001f

enum Material {
    DIFFUSE,
    MIRROR,
    GLASS
};

struct Shape {
    Vector color, emit;
    Material material;  // Add material attribute

    Shape(const Vector &color_, const Vector &emit_, Material material_) 
        : color(color_), emit(emit_), material(material_) {}

    virtual double intersects(const Ray &r) const { return 0; }
    virtual Vector randomPoint() const { return Vector(); }
    virtual Vector getNormal(const Vector &p) const { return Vector(); }
    virtual Vector getColor(const Vector &p) const { return color; }
    virtual Vector getEmission() const { return emit; }
};

struct Sphere : Shape {
    Vector center;
    double radius;

    Sphere(const Vector &center_, double radius_, const Vector &color_, const Vector &emit_, Material material_)
        : Shape(color_, emit_, material_), center(center_), radius(radius_) {}

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

struct Cube : Shape {
    Vector min, max, center;
    double angle;  // Rotation angle in radians

    Cube(const Vector &min_, const Vector &max_, const Vector &color_, const Vector &emit_, Material material_, double angle_)
        : Shape(color_, emit_, material_), min(min_), max(max_), center((min_ + max_) / 2), angle(angle_) {}

    Vector rotatePoint(const Vector &p) const {
        Vector translated = p - center;
        double sinAngle = sin(angle);
        double cosAngle = cos(angle);

        double x = translated.x * cosAngle - translated.z * sinAngle;
        double z = translated.x * sinAngle + translated.z * cosAngle;

        return Vector(x, translated.y, z) + center;
    }

    double intersects(const Ray &r) const override {
        Ray rotatedRay(rotatePoint(r.origin), rotatePoint(r.direction + r.origin) - rotatePoint(r.origin));

        double tmin = (min.x - rotatedRay.origin.x) / rotatedRay.direction.x;
        double tmax = (max.x - rotatedRay.origin.x) / rotatedRay.direction.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        double tymin = (min.y - rotatedRay.origin.y) / rotatedRay.direction.y;
        double tymax = (max.y - rotatedRay.origin.y) / rotatedRay.direction.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax + EPSILON) || (tymin > tmax + EPSILON)) return 0;
        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;

        double tzmin = (min.z - rotatedRay.origin.z) / rotatedRay.direction.z;
        double tzmax = (max.z - rotatedRay.origin.z) / rotatedRay.direction.z;
        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax + EPSILON) || (tzmin > tmax + EPSILON)) return 0;
        if (tzmin > tmin) tmin = tzmin;
        if (tzmax < tmax) tmax = tzmax;

        return tmin > EPSILON ? tmin : (tmax > EPSILON ? tmax : 0);
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

struct Plane : Shape {
    Vector normal;
    double d;

    Plane(const Vector &normal_, double d_, const Vector &color_, const Vector &emit_, Material material_)
        : Shape(color_, emit_, material_), normal(normal_), d(d_) {}

    double intersects(const Ray &r) const override {
        double denom = normal.dot(r.direction);
        if (fabs(denom) < EPSILON) return 0;
        double t = -(r.origin.dot(normal) + d) / denom;
        return (t > EPSILON) ? t : 0;
    }

    Vector getNormal(const Vector &p) const override {
        return normal;
    }

    Vector randomPoint() const override {
        return Vector();  // Not used for planes
    }
};

struct Checkerboard : Plane {
    Vector color1, color2;
    double size;

    Checkerboard(const Vector &normal_, double d_, const Vector &color1_, const Vector &color2_, double size_, const Vector &emit_, Material material_)
        : Plane(normal_, d_, Vector(), emit_, material_), color1(color1_), color2(color2_), size(size_) {}

    Vector getColor(const Vector &p) const override {
        Vector localP = p - normal * (p.dot(normal) + d); // Project p onto the plane
        int x, y;

        // Determine the dominant axis of the normal vector
        if (fabs(normal.x) > fabs(normal.y) && fabs(normal.x) > fabs(normal.z)) {
            // Plane is yz
            x = floor(localP.y / size);
            y = floor(localP.z / size);
        } else if (fabs(normal.y) > fabs(normal.x) && fabs(normal.y) > fabs(normal.z)) {
            // Plane is xz
            x = floor(localP.x / size);
            y = floor(localP.z / size);
        } else {
            // Plane is xy
            x = floor(localP.x / size);
            y = floor(localP.y / size);
        }

        if ((x + y) % 2 == 0) {
            return color1;
        } else {
            return color2;
        }
    }
};

struct Stripe : Plane {
    Vector color1, color2;
    double size;

    Stripe(const Vector &normal_, double d_, const Vector &color1_, const Vector &color2_, double size_, const Vector &emit_, Material material_)
        : Plane(normal_, d_, Vector(), emit_, material_), color1(color1_), color2(color2_), size(size_) {}

    Vector getColor(const Vector &p) const override {
        Vector localP = p - normal * (p.dot(normal) + d); // Project p onto the plane
        int stripe;

        // Determine the dominant axis of the normal vector
        if (fabs(normal.x) > fabs(normal.y) && fabs(normal.x) > fabs(normal.z)) {
            // Plane is yz
            stripe = floor(localP.y / size);
        } else if (fabs(normal.y) > fabs(normal.x) && fabs(normal.y) > fabs(normal.z)) {
            // Plane is xz
            stripe = floor(localP.x / size);
        } else {
            // Plane is xy
            stripe = floor(localP.x / size);
        }

        if (stripe % 2 == 0) {
            return color1;
        } else {
            return color2;
        }
    }
};


#endif // SHAPES_H
