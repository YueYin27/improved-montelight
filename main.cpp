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
    new Plane(Vector(1, 0, 0), 0, Vector(0.9, 0.6, 0.7) * 0.799, Vector(), DIFFUSE),  // Left wall
    new Plane(Vector(-1, 0, 0), 100, Vector(0.2, 0.6, 0.86), Vector(), DIFFUSE),  // Right wall
    new Plane(Vector(0, -1, 0), 81.6, Vector(176, 159, 202) / 255.0, Vector(), DIFFUSE),  // Ceiling
    new Stripe(Vector(0, 0, -1), 0, Vector(0.75, 0.75, 0.25), Vector(0.5, 0.25, 0.5), 10, Vector(), DIFFUSE),  // Back Wall
    new Checkerboard(Vector(0, 1, 0), 0, Vector(0.25, 0.25, 0.25), Vector(1, 1, 1), 10, Vector(), DIFFUSE),  // Checkerboard (Floor)
    new Sphere(Vector(25, 16.5, 45), 16.5f, Vector(1, 1, 1), Vector(), MIRROR),  // Small sphere
    new Sphere(Vector(50, 12, 100), 12, Vector(1, 1, 1), Vector(), GLASS),  // Small sphere
    new Cube(Vector(60, 0, 60), Vector(85, 40, 85), Vector(0.9, 0.6, 0.7) * 0.799, Vector(), DIFFUSE, M_PI / 4),  // Small cube
    new Sphere(Vector(50, 73, 81.6), 5, Vector(), Vector(4, 4, 4) * 100, DIFFUSE)  // Light source
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

    // Default values
    int w = 256, h = 256;
    unsigned int MAX_spp = 100;
    unsigned int MIN_spp = 30;
    bool adaptive_sampling = false;

    // Check if command line arguments are provided
    if (argc >= 4) {
        w = std::stoi(argv[1]);
        h = std::stoi(argv[2]);
        adaptive_sampling = std::string(argv[3]) == "true";
        if (argc == 6) {
            MAX_spp = std::stoi(argv[4]);
            MIN_spp = std::stoi(argv[5]);
        }
    } else if(argc > 6) {
        std::cout << "Usage: " << argv[0] << " <width> <height> <adaptive_sampling> [<max_spp> <min_spp>]" << std::endl;
        return 1;
    }

    int SNAPSHOT_INTERVAL = 10;
    bool FOCUS_EFFECT = false;
    double FOCAL_LENGTH = 35;
    double APERTURE_FACTOR = 1;
    Image img(w, h);
    auto &scene = simpleScene;
    double aperture = 0.5135 / APERTURE_FACTOR;
    Vector cx = Vector((w * aperture) / h, 0, 0);
    Vector dir_norm = Vector(0, -0.042612, -1).normalize();
    double L = 140;
    double L_new = APERTURE_FACTOR * L;
    double L_diff = L - L_new;
    Vector cam_shift = dir_norm * (L_diff);
    if (L_diff < 0){
        cam_shift = cam_shift * 1.5;
    }
    L = L_new;
    Ray camera = Ray(Vector(50, 52, 295.6) + cam_shift, dir_norm);
    Tracer tracer = Tracer(scene, camera.origin);
    Vector cy = (cx.cross(camera.direction)).normalize() * aperture;

    auto start = std::chrono::high_resolution_clock::now();

    // Define a constant for maximum acceptable variance
    const double MAX_VARIANCE = 1e-3; // Adjust based on desired quality

    for (int sample = 1; sample <= MAX_spp; ++sample) {
        printProgressBar(sample, MAX_spp);
        if (sample && sample % SNAPSHOT_INTERVAL == 0) {
            std::ostringstream fn;
            fn << std::setfill('0') << std::setw(5) << sample;
            img.save("results_temp/render_" + fn.str());
        }
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                unsigned int index = (h - y - 1) * w + x;
                if (adaptive_sampling && img.samples[index] > MIN_spp && img.variance[index] < MAX_VARIANCE) {
                    continue; // Skip sampling if variance is low enough
                }
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
                Ray ray = Ray(camera.origin + d * 140, d.normalize());
                if (FOCUS_EFFECT) {
                    Vector fp = (camera.origin + d * L) + d.normalize() * FOCAL_LENGTH;
                    Vector del_x = (cx * dx * L / float(w));
                    Vector del_y = (cy * dy * L / float(h));
                    Vector point = camera.origin + d * L;
                    point = point + del_x + del_y;
                    d = (fp - point).normalize();
                    ray = Ray(camera.origin + d * L, d.normalize());
                }
                Vector rads = tracer.getRadiance(ray, 0);
                rads.clamp();
                img.setPixel(x, y, rads);
            }
        }
    }

    printProgressBar(MAX_spp, MAX_spp); // Ensure progress bar shows 100%

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "\nRendering completed in " << elapsed.count() << " seconds." << std::endl;

    img.save("results_final/render");
    return 0;
}
