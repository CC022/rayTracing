//
//  renderer.hpp
//  rayTracing
//
//  Created by zzc on 4/18/21.
//  Copyright © 2021 zzc. All rights reserved.
//

#ifndef renderer_hpp
#define renderer_hpp

#include <stdio.h>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include "vec3.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "sphere.hpp"
#include "triangle.hpp"
#include "hittableList.hpp"
#include "random.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "aabb.hpp"
#include "bvh.hpp"
#include "texture.hpp"
#include "bvh.hpp"

class renderer {
    size_t width = 0;
    size_t height = 0;
    size_t samples = 0;
    std::vector<std::thread> threads;
    int threadsCount = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 8;
    hittable *world = nullptr;
    camera cam;
    std::vector<vec3> canvas;
    
    static vec3 color(const ray& r, hittable *world, int depth) {
        hit_record rec;
        if (world->hit(r, 0.001, MAXFLOAT, rec)) {
            ray scattered;
            vec3 attenuation;
            if (depth < 12 && rec.matPtr->scatter(r, rec, attenuation, scattered)) {
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

    static void renderKernel(int threadIDx, int threadIDy, int width, int height, int samples, camera &cam, hittable *world, std::vector<vec3> &canvas) {
        int x = threadIDx;
        int y = threadIDy;
        if (x > width or y > height) return;
        vec3 col(0,0,0);
        for (int sample = 0; sample < samples; sample++) {
            float u = float(x + randomFloat()) / float(width);
            float v = float(y + randomFloat()) / float(height);
            ray r = cam.getRay(u, v);
            col += color(r, world, 0);
        }
        col /= float(samples);
        col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
        canvas[y * width + x] = col;
    }
    
public:
    renderer(size_t width, size_t height, size_t samples, vec3 lookFrom, vec3 lookAt, float vfov, float aperture) :
    width(width),
    height(height),
    samples(samples),
    cam(lookFrom, lookAt, vec3(0, 1, 0), vfov, float(width)/float(height), aperture, (lookFrom-lookAt).length())
    {
        canvas.resize(width*height);
    }
    
    void loadWorld(hittableList world) {
        static bvhNode bvhWorld = bvhNode(world);
        this->world = &bvhWorld;
    }
    
    void render() {
        auto startTime = std::chrono::steady_clock::now();
        for (ssize_t j = height-1; j >= 0; j--) {
            std::cout << "\rRendering progress " << (height - j) * 100 / height << '%' << std::flush;
            for (ssize_t i = 0; i < width; i++) {
                threads.emplace_back(std::thread(renderKernel, i, j, width, height, samples, std::ref(cam), world, std::ref(canvas)));
                if (threads.size() >= threadsCount) {
                    for (std::thread &thread: threads) {thread.join();}
                    threads.clear();
                }
            }
        }
        for (std::thread &thread: threads) {thread.join();}
        auto stopTime = std::chrono::steady_clock::now();
        std::cout << "\nRendering took " << std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count() << " ms\n";
    }
    
    void writeToPPMFile(std::string file) {
        std::ofstream outputFile(file);
        outputFile << "P3\n" << width << " " << height << "\n255\n";
        for (int j = height-1; j >= 0; j--) {
            for (int i = 0; i < width; i++) {
                vec3 &pixel = canvas[j * width + i];
                outputFile << int(255.99 * pixel[0]) << " " << int(255.99 * pixel[1]) << " " << int(255.99 * pixel[2]) << "\n";
            }
        }
    }
};
#endif /* renderer_hpp */
