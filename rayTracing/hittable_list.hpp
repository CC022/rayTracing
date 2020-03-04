//
//  hittable_list.hpp
//  rayTracing
//
//  Created by zzc on 3/3/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#ifndef hittable_list_hpp
#define hittable_list_hpp

#include <stdio.h>
#include "hittable.hpp"

class hittable_list: public hittable {
public:
    hittable **list;
    int list_size;
    
    hittable_list() {}
    hittable_list(hittable **l, int n) {list = l; list_size = n;}
    
    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        hit_record temp_rec;
        bool hitAnything = false;
        double closestSoFar = t_max;
        for (int i=0; i<list_size; i++) {
            if (list[i]->hit(r, t_min, closestSoFar, temp_rec)) {
                hitAnything = true;
                closestSoFar = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hitAnything;
    }
    
};


#endif /* hittable_list_hpp */
