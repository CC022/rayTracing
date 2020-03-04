//
//  main.cpp
//  rayTracing
//
//  Created by zzc on 3/2/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "vec3.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "sphere.hpp"
#include "hittable_list.hpp"
//
//float hit_sphere(const vec3& center, float radius, const ray& r) {
//    vec3 oc = r.origin() - center;
//    float a = dot(r.direction(), r.direction());
//    float b = 2.0 * dot(oc, r.direction());
//    float c = dot(oc, oc) - radius*radius;
//    float discriminant  = b*b - 4*a*c;
//    if (discriminant < 0) {
//        return -1;
//    } else {
//        return (-b - sqrt(discriminant)) / (2.0 * a);
//    }
//}

vec3 color(const ray& r, hittable *world) {
    hit_record rec;
    if (world->hit(r, 0.0, MAXFLOAT, rec)) {
        return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

int main(int argc, const char * argv[]) {
    using namespace std;
    ofstream imgFile("img.ppm");
    int width = 2000;
    int height = 1000;
    imgFile << "P3\n" << width << " " << height << "\n255\n";
    vec3 lowerLeftCorner(-2.0,-1.0,-1.0);
    vec3 horizontal(4.0,0.0,0.0);
    vec3 vertical(0.0,2.0,0.0);
    vec3 origin(0.0,0.0,0.0);
    
    hittable *list[2];
    list[0] = new sphere(vec3(0, 0, -1), 0.5);
    list[1] = new sphere(vec3(0, -100.5, -1), 100);
    hittable *world = new hittable_list(list, 2);
    
    for (int j = height-1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
            float u = float(i) / float(width);
            float v = float(j) / float(height);
            ray r(origin, lowerLeftCorner + u*horizontal + v*vertical);
            vec3 p = r.pointAtParameter(2.0);
            vec3 col = color(r, world);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            imgFile << ir << " " << ig << " " << ib << "\n";
        }
    }
    return 0;
}
