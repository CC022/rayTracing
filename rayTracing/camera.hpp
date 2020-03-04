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

class camera {
public:
    vec3 lowerLeftCorner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
    
    camera() {
        lowerLeftCorner = vec3(-2.0, -1.0, -1.0);
        horizontal = vec3(4.0, 0.0, 0.0);
        vertical = vec3(0.0, 2.0, 0.0);
        origin = vec3(0.0, 0.0, 0.0);
    }
    
    ray getRay(float u, float v) {
        return ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
    }
};

#endif /* camera_hpp */
