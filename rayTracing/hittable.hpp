#pragma once
#include <stdio.h>
#include "ray.hpp"
#include "aabb.hpp"

class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material *matPtr;
};

class hittable {
public:
    virtual bool hit (const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool boundingBox(aabb& outputBox) const = 0;
};
