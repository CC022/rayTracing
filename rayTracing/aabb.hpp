#pragma once
#include <stdio.h>
#include "ray.hpp"
#include "vec3.hpp"

class aabb {
    vec3 minimum;
    vec3 maximum;
    
public:
    aabb() : minimum{0,0,0}, maximum{0,0,0} {}
    aabb(const vec3 &a, const vec3 &b) : minimum(a), maximum(b) {}
    
    vec3 min() const {return minimum;}
    vec3 max() const {return maximum;}
    
    bool hit(const ray &r, float tMin, float tMax) const {
        for (int dim = 0; dim < 3; dim++) {
            float invD = 1.0f / r.direction()[dim];
            float t0 = (minimum[dim] - r.origin()[dim]) * invD;
            float t1 = (maximum[dim] - r.origin()[dim]) * invD;
            if (invD < 0.0f) {std::swap(t0, t1);}
            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax <= tMin) return false;
        }
        return true;
    }
};

inline aabb surroundingBox(aabb const &box0, aabb const &box1) {
    vec3 small(fmin(box0.min().x(), box1.min().x()),
               fmin(box0.min().y(), box1.min().y()),
               fmin(box0.min().z(), box1.min().z()));
    vec3 big(fmax(box0.max().x(), box1.max().x()),
             fmax(box0.max().y(), box1.max().y()),
             fmax(box0.max().z(), box1.max().z()));
    return aabb(small, big);
}
