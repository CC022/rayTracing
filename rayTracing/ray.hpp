//
//  ray.hpp
//  rayTracing
//
//  Created by zzc on 3/2/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#ifndef ray_hpp
#define ray_hpp

#include <stdio.h>
#include "vec3.hpp"

class ray {
    
public:
    ray() {}
    ray(const vec3& origin, const vec3& direction) {orig = origin; dir = direction;}
    vec3 origin() const {return orig;}
    vec3 direction() const {return dir;}
    vec3 at(float t) const {return orig + t*dir;}
    
    vec3 orig;
    vec3 dir;
};

#endif /* ray_hpp */
