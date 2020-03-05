//
//  sphere.hpp
//  rayTracing
//
//  Created by zzc on 3/3/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#ifndef sphere_hpp
#define sphere_hpp

#include <stdio.h>
#include "hittable.hpp"
#include "ray.hpp"
#include "random.hpp"

class sphere: public hittable {
public:
    vec3 center;
    float radius;
    material *matPtr;
    
    sphere() {}
    sphere(vec3 cen, float r, material *m) : center(cen), radius(r), matPtr(m) {};
    
    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        vec3 oc = r.origin() - center;
        float a = dot(r.direction(), r.direction());
        float b = dot(oc, r.direction());
        float c = dot(oc, oc) - radius*radius;
        float discriminant = b*b - a*c;
        if (discriminant > 0) {
            float temp = (-b - sqrt(discriminant))/a;
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.pointAtParameter(rec.t);
                rec.normal = (rec.p - center) / radius;
                rec.matPtr = matPtr;
                return true;
            }
            temp = (-b + sqrt(discriminant)) / a;
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.pointAtParameter(rec.t);
                rec.normal = (rec.p - center) / radius;
                rec.matPtr = matPtr;
                return true;
            }
        }
        return false;
    }
};

vec3 randomInUnitSphere() {
    vec3 p;
    do {
        p = 2.0*vec3(randomDouble(), randomDouble(), randomDouble()) - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0);
    return p;
}

#endif /* sphere_hpp */
