#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include "vector.h"
#include "ray.h"
#include "image.h"
#include "shape.h"
#include "sphere.h"
#include "tracer.h"

bool EMITTER_SAMPLING = true;

std::vector<Shape *> simpleScene = {
    new Sphere(Vector(1e5+1,40.8,81.6), 1e5f, Vector(.75,.25,.25), Vector()),
    new Sphere(Vector(-1e5+99,40.8,81.6), 1e5f, Vector(.25,.25,.75), Vector()),
    new Sphere(Vector(50,40.8, 1e5), 1e5f, Vector(.75,.75,.75), Vector()),
    new Sphere(Vector(50, 1e5, 81.6), 1e5f, Vector(.75,.75,.75), Vector()),
    new Sphere(Vector(50,-1e5+81.6,81.6), 1e5f, Vector(.75,.75,.75), Vector()),
    new Sphere(Vector(27,16.5,47), 16.5f, Vector(1,1,1) * 0.799, Vector()),
    new Sphere(Vector(73,16.5,78), 16.5f, Vector(1,1,1) * 0.799, Vector()),
    new Sphere(Vector(50,65.1,81.6), 8.5, Vector(), Vector(4,4,4) * 100)
};

std::vector<Shape *> complexScene = {
    new Sphere(Vector(1e5+1,40.8,81.6), 1e5f, Vector(.75,.25,.25), Vector()),
    new Sphere(Vector(-1e5+99,40.8,81.6), 1e5f, Vector(.25,.25,.75), Vector()),
    new Sphere(Vector(50,40.8, 1e5), 1e5f, Vector(.75,.75,.75), Vector()),
    new Sphere(Vector(50, 1e5, 81.6), 1e5f, Vector(.75,.75,.75), Vector()),
    new Sphere(Vector(50,-1e5+81.6,81.6), 1e5f, Vector(.75,.75,.75), Vector()),
    new Sphere(Vector(20,16.5,40), 16.5f, Vector(1,1,1) * 0.799, Vector()),
    new Sphere(Vector(50,16.5,80), 16.5f, Vector(1,1,1) * 0.799, Vector()),
    new Sphere(Vector(75,16.5,120), 16.5f, Vector(1,1,1) * 0.799, Vector()),
    new Sphere(Vector(50,65.1,40), 1.5, Vector(), Vector(4,4,4) * 100),
    new Sphere(Vector(50,65.1,120), 1.5, Vector(), Vector(4,4,4) * 100)
};

int main(int argc, const char *argv[]) {
    EMITTER_SAMPLING = true;
    int w = 256, h = 256;
    int SNAPSHOT_INTERVAL = 10;
    unsigned int SAMPLES = 50;
    bool FOCUS_EFFECT = false;
    double FOCAL_LENGTH = 35;
    double APERTURE_FACTOR = 1;
    Image img(w, h);
    auto &scene = complexScene;
    Tracer tracer = Tracer(scene);
    double aperture = 0.5135 / APERTURE_FACTOR;
    Vector cx = Vector((w * aperture) / h, 0, 0);
    Vector dir_norm = Vector(0, -0.042612, -1).norm();
    double L = 140;
    double L_new = APERTURE_FACTOR * L;
    double L_diff = L - L_new;
    Vector cam_shift = dir_norm * (L_diff);
    if (L_diff < 0){
        cam_shift = cam_shift * 1.5;
    }
    L = L_new;
    Ray camera = Ray(Vector(50, 52, 295.6) + cam_shift, dir_norm);
    Vector cy = (cx.cross(camera.direction)).norm() * aperture;

    for (int sample = 0; sample < SAMPLES; ++sample) {
        std::cout << "Taking sample " << sample << "\r" << std::flush;
        if (sample && sample % SNAPSHOT_INTERVAL == 0) {
            std::ostringstream fn;
            fn << std::setfill('0') << std::setw(5) << sample;
            img.save("temp/render_" + fn.str());
        }
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                double Ux = 2 * drand48();
                double Uy = 2 * drand48();
                double dx;
                if (Ux < 1) {
                    dx = sqrt(Ux) - 1;
                } else {
                    dx = 1 - sqrt(2 - Ux);
                }
                double dy;
                if (Uy < 1) {
                    dy = sqrt(Uy) - 1;
                } else {
                    dy = 1 - sqrt(2 - Uy);
                }
                Vector d = (cx * (((x+dx) / float(w)) - 0.5)) + (cy * (((y+dy) / float(h)) - 0.5)) + camera.direction;
                Ray ray = Ray(camera.origin + d * 140, d.norm());
                if (FOCUS_EFFECT) {
                    Vector fp = (camera.origin + d * L) + d.norm() * FOCAL_LENGTH;
                    Vector del_x = (cx * dx * L / float(w));
                    Vector del_y = (cy * dy * L / float(h));
                    Vector point = camera.origin + d * L;
                    point = point + del_x + del_y;
                    d = (fp - point).norm();
                    ray = Ray(camera.origin + d * L, d.norm());
                }
                Vector rads = tracer.getRadiance(ray, 0);
                rads.clamp();
                img.setPixel(x, y, rads);
            }
        }
    }
    img.save("render");
    return 0;
}
