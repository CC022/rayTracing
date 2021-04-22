//
//  triangle.hpp
//  rayTracing
//
//  Created by zzc on 4/18/21.
//  Copyright © 2021 zzc. All rights reserved.
//

#ifndef triangle_hpp
#define triangle_hpp

#include <stdio.h>
#include <cmath>
#include "hittable.hpp"
#include "ray.hpp"
#include "random.hpp"

class triangle : public hittable {
    
public:
    vec3 vertices[3];
    float u[3];
    float v[3];
    material *matPtr;
    
    triangle(vec3 v0, vec3 v1, vec3 v2, material *m) : matPtr(m) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
    }
    
    triangle(vec3 v0, vec3 v1, vec3 v2, float U0, float U1, float U2, float V0, float V1, float V2, material *m) : matPtr(m) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
        u[0] = U0; u[1] = U1; u[2] = U2;
        v[0] = V0; v[1] = V1; v[2] = V2;
    }
    
    bool hit(const ray& r, float t_min, float t_max, hit_record &rec) const override {
        vec3 e1 = vertices[1] - vertices[0];
        vec3 e2 = vertices[2] - vertices[0];
        vec3 s = r.origin() - vertices[0];
        vec3 s1 = cross(r.direction(), e2);
        vec3 s2 = cross(s, e1);
        
        vec3 Normal = cross(e1, e2);
        if (fabs(dot(r.direction(), Normal)) < __FLT_EPSILON__) {
            return false; // triangle plane is parallel to ray direction
        }
        float t = dot(s2, e2) / dot(s1, e1);
        float b1 = dot(s1, s) / dot(s1, e1);
        float b2 = dot(s2, r.direction()) / dot(s1, e1);
        if (b1 < 0 or b2 < 0 or b1+b2 > 1) {return false;}
        if (t < t_max and t > t_min) {
            rec.t = t;
            rec.p = r.at(rec.t);
            rec.normal = Normal;
            // TODO: real UV
            rec.u = b1*u[0] + b2*u[1] + (1-b1-b2)*u[2];
            rec.v = b1*v[0] + b2*v[1] + (1-b1-b2)*v[2];
            rec.matPtr = matPtr;
            return true;
        }
        return false;
    }
    
    bool boundingBox(aabb &outputBox) const override {
        float x_min = fmin(fmin(vertices[0].x(), vertices[1].x()), vertices[2].x());
        float x_max = fmax(fmax(vertices[0].x(), vertices[1].x()), vertices[2].x());
        float y_min = fmin(fmin(vertices[0].y(), vertices[1].y()), vertices[2].y());
        float y_max = fmax(fmax(vertices[0].y(), vertices[1].y()), vertices[2].y());
        float z_min = fmin(fmin(vertices[0].z(), vertices[1].z()), vertices[2].z());
        float z_max = fmax(fmax(vertices[0].z(), vertices[1].z()), vertices[2].z());
        outputBox = aabb(vec3(x_min, y_min, z_min), vec3(x_max, y_max, z_max));
        return true;
    }
    
};

#endif /* triangle_hpp */
