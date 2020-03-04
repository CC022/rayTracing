//
//  hittable.hpp
//  rayTracing
//
//  Created by zzc on 3/3/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#ifndef hittable_hpp
#define hittable_hpp

#include <stdio.h>
#include "ray.hpp"

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
};

class hittable {
public:
    virtual bool hit (const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif /* hittable_hpp */
