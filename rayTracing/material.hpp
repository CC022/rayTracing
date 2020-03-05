//
//  material.hpp
//  rayTracing
//
//  Created by zzc on 3/3/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#ifndef material_hpp
#define material_hpp

#include <stdio.h>
#include "sphere.hpp"

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v, n)*n;
}

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian: public material {
public:
    vec3 albedo;
    
    lambertian(const vec3& a): albedo(a) {}
    bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const override {
        vec3 target = rec.p + rec.normal + randomInUnitSphere();
        scattered = ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }
};

class metal: public material {
public:
    vec3 albedo;
    
    metal(const vec3& a):albedo(a) {}
    bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
};


#endif /* material_hpp */
