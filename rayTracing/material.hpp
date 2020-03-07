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

bool refract(const vec3& v, const vec3& n, float niOverNt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - niOverNt*niOverNt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = niOverNt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    return false;
}

float schlick(float cosine, float refIdx) {
    float r0 = (1-refIdx) / (1+refIdx);
    r0 = r0 * r0;
    return r0 + (1-r0)*pow((1-cosine), 5);
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

class dielectric: public material {
public:
    float refIdx;
    dielectric(float ri): refIdx(ri) {}
    bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const override {
        vec3 outwardNormal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float niOverNt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        
        float reflectProb;
        float cosine;
        
        if (dot(r_in.direction(), rec.normal) > 0) {
            outwardNormal = -rec.normal;
            niOverNt = refIdx;
            cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = sqrt(1 - refIdx*refIdx*(1-cosine*cosine));
        } else {
            outwardNormal = rec.normal;
            niOverNt = 1.0 / refIdx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outwardNormal, niOverNt, refracted)) {
            reflectProb = schlick(cosine, refIdx);
        } else {
            reflectProb = 1.0;
        }
        
        if (randomDouble() < reflectProb) {
            scattered = ray(rec.p, reflected);
        } else {
            scattered = ray(rec.p, refracted);
        }
        return true;
    }
};

#endif /* material_hpp */
