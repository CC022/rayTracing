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
    ray(const vec3& a, const vec3& b) {A = a; B = b;}
    vec3 origin() const {return A;}
    vec3 direction() const {return B;}
    vec3 pointAtParameter(float t) const {return A + t*B;}
    
    vec3 A;
    vec3 B;
};

#endif /* ray_hpp */
