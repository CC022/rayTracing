//
//  main.cpp
//  rayTracing
//
//  Created by zzc on 3/2/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#include <iostream>

#include "renderer.hpp"
#include "sphere.hpp"

hittableList random_scene() {
    int n = 500;
    hittable **list = new hittable*[n+1];
    int i = 0;
    list[i++] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(new checkerTexture(vec3(0.7, 0.7, 0.7), vec3(0.9, 0.9, 0.9))));
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = randomFloat();
            vec3 center(a+0.9*randomFloat(),0.2,b+0.9*randomFloat());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2,
                                           new lambertian(vec3(randomFloat()*randomFloat(),
                                                               randomFloat()*randomFloat(),
                                                               randomFloat()*randomFloat())
                                                          )
                                           );
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                                           new metal(vec3(0.5*(1 + randomFloat()),
                                                          0.5*(1 + randomFloat()),
                                                          0.5*(1 + randomFloat()))));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    texture* earthTexture = new imageTexture("/Volumes/DATA/Documents_local/earthmap.ppm");
    list[i++] = new sphere(vec3(2, 0.9, 4), 1.0, new lambertian(earthTexture));
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.9, 0.9, 0.8)));
    list[i++] = new triangle(vec3(2, 1, -5), vec3(0, 3, -5), vec3(-2, 1, -5), new lambertian(vec3(0.5, 0.0, 0.0)));
    return hittableList(list, i);
}



int main(int argc, const char * argv[]) {
    using namespace std;
    int width = 600;
    int height = 300;
    int samples = 16;
    hittableList randomScene = random_scene();
    vec3 lookfrom(12,3,6);
    vec3 lookat(0,0,0);
    renderer renderer0(width, height, samples, lookfrom, lookat, 40, 0.1);
    renderer0.loadWorld(randomScene);
    renderer0.render();
    renderer0.writeToPPMFile("img.ppm");
    return 0;
}
