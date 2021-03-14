//
//  main.cpp
//  rayTracing
//
//  Created by zzc on 3/2/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include "vec3.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "sphere.hpp"
#include "hittable_list.hpp"
#include "random.hpp"
#include "camera.hpp"
#include "material.hpp"

hittable *random_scene() {
    int n = 500;
    hittable **list = new hittable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = randomDouble();
            vec3 center(a+0.9*randomDouble(),0.2,b+0.9*randomDouble());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2,
                                           new lambertian(vec3(randomDouble()*randomDouble(),
                                                               randomDouble()*randomDouble(),
                                                               randomDouble()*randomDouble())
                                                          )
                                           );
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                                           new metal(vec3(0.5*(1 + randomDouble()),
                                                          0.5*(1 + randomDouble()),
                                                          0.5*(1 + randomDouble()))));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5)));
    
    return new hittable_list(list,i);
}

vec3 color(const ray& r, hittable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.matPtr->scatter(r, rec, attenuation, scattered)) {
            return attenuation*color(scattered, world, depth+1);
        } else {
            return vec3(0, 0, 0);
        }
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

void renderKernel(int threadIDx, int threadIDy, int width, int height, int samples, camera &cam, hittable *world, std::vector<vec3> &canvas) {
    int x = threadIDx;
    int y = threadIDy;
    if (x > width or y > height) return;
    vec3 col(0,0,0);
    float u = float(x + randomFloat()) / float(width);
    float v = float(y + randomFloat()) / float(height);
    ray r = cam.getRay(u, v);
    for (int sample = 0; sample < samples; sample++) {
        col += color(r, world, 0);
    }
    col /= float(samples);
    col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
    canvas[y * width + x] = col;
}

int main(int argc, const char * argv[]) {
    using namespace std;
    ofstream imgFile("img.ppm");
    int width = 1920;
    int height = 1080;
    int samples = 100;
    std::vector<std::thread> threads;
    std::mutex colorWriteMutex;
    int threadsCount = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 8;
    imgFile << "P3\n" << width << " " << height << "\n255\n";
    vec3 lowerLeftCorner(-2.0,-1.0,-1.0);
    vec3 horizontal(4.0,0.0,0.0);
    vec3 vertical(0.0,2.0,0.0);
    vec3 origin(0.0,0.0,0.0);
    
    hittable *world = random_scene();
    vec3 lookfrom(12,3,6);
    vec3 lookat(0,0,0);
    float distToFocus = (lookfrom - lookat).length();
    float aperture = 0.1;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 40, float(width) / float(height), aperture, distToFocus);
    std::vector<vec3> canvas;
    canvas.resize(width*height);
    
    for (int j = height-1; j >= 0; j--) {
        std::cout << "\rRendering progress " << (height - j) * 100 / height << '%' << std::flush;
        for (int i = 0; i < width; i++) {
            threads.emplace_back(std::thread(renderKernel, i, j, width, height, samples, std::ref(cam), world, std::ref(canvas)));
            if (threads.size() >= threadsCount) {
                for (std::thread &thread: threads) {thread.join();}
                threads.clear();
            }
        }
    }
    for (std::thread &thread: threads) {thread.join();}
    
    for (int j = height-1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
            vec3 pixel = canvas[j * width + i];
            imgFile << int(255.99 * pixel[0]) << " " << int(255.99 * pixel[1]) << " " << int(255.99 * pixel[2]) << "\n";
        }
    }
    
    return 0;
}
