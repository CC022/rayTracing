//
//  camera.hpp
//  rayTracing
//
//  Created by zzc on 3/3/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#ifndef camera_hpp
#define camera_hpp

#include <stdio.h>
#include "ray.hpp"
#include "random.hpp"

vec3 randomInUnitDisk() {
    vec3 p;
    do {
        p = 2.0*vec3(randomDouble(), randomDouble(), 0) - vec3(1, 1, 0);
    } while (dot(p,p) >= 1.0);
    return p;
}

class camera {
public:
    vec3 lowerLeftCorner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
    vec3 u, v, w;
    float lensRadius;
    
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focusDist) {
        lensRadius = aperture/2;
        float theta = vfov*M_PI/180;
        float halfHeight = tan(theta/2);
        float halfWidth = aspect * halfHeight;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        lowerLeftCorner = origin - halfWidth*focusDist*u - halfHeight*focusDist*v - focusDist*w;
        horizontal = 2*halfWidth*focusDist*u;
        vertical = 2*halfHeight*focusDist*v;
    }
    
    ray getRay(float s, float t) {
        vec3 rd = lensRadius * randomInUnitDisk();
//        std::cout << "rd " << offset.length() << std::endl;

        vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - origin - offset);
    }
};

#endif /* camera_hpp */
