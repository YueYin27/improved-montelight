#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <chrono>
#include "vector.h"
#include "ray.h"
#include "image.h"
#include "shapes.h"
#include "tracer.h"

bool EMITTER_SAMPLING = true;

std::vector<Shape *> simpleScene = {
    new Plane(Vector(1, 0, 0), 0, Vector(0.9, 0.6, 0.7) * 0.799, Vector()),  // Left wall
    new Plane(Vector(-1, 0, 0), 100, Vector(0.2, 0.6, 0.86), Vector()),  // Right wall
    new Plane(Vector(0, 0, -1), 0, Vector(200, 196, 223) / 255.0, Vector()),  // Back wall
    new Plane(Vector(0, -1, 0), 81.6, Vector(176, 159, 202) / 255.0, Vector()),  // Ceiling
    new Checkerboard(Vector(0.5, 0.25, 0.5), Vector(0.75, 0.75, 0.25), 10, Vector()),  // Checkerboard (Floor)
    new Sphere(Vector(27, 16.5, 47), 16.5f, Vector(47, 83, 155) / 255.0, Vector()),  // Small sphere
    new Cube(Vector(60, 0, 60), Vector(85, 40, 85), Vector(0.9, 0.6, 0.7) * 0.799, Vector(), M_PI/4), // Big cube
    new Sphere(Vector(50, 73, 81.6), 5, Vector(), Vector(4, 4, 4) * 150)  // Light source
};

void printProgressBar(int sample, int totalSamples) {
    int barWidth = 70;
    float progress = float(sample) / totalSamples;
    std::cout << "Sample " << sample << "/" << totalSamples << " ";
    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

int main(int argc, const char *argv[]) {
    srand48(26);  // set random seed
    EMITTER_SAMPLING = true;
    int w = 256, h = 256;
    int SNAPSHOT_INTERVAL = 10;
    unsigned int SAMPLES = 50;
    bool FOCUS_EFFECT = false;
    double FOCAL_LENGTH = 35;
    double APERTURE_FACTOR = 1;
    Image img(w, h);
    auto &scene = simpleScene;
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

    auto start = std::chrono::high_resolution_clock::now();

    for (int sample = 1; sample <= SAMPLES; ++sample) {
        printProgressBar(sample, SAMPLES);
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
                Vector d = (cx * (((x + dx) / float(w)) - 0.5)) + (cy * (((y + dy) / float(h)) - 0.5)) + camera.direction;
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

    printProgressBar(SAMPLES, SAMPLES); // Ensure progress bar shows 100%

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "\nRendering completed in " << elapsed.count() << " seconds." << std::endl;

    img.save("render");
    return 0;
}
