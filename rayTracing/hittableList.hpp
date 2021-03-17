#pragma once
#include <stdio.h>
#include "hittable.hpp"
#include "aabb.hpp"

class hittableList: public hittable {
public:
    hittable **list;
    int listSize;
    
    hittableList() {}
    hittableList(hittable *object) {list = new hittable*[1]; list[0] = object; listSize = 1;}
    hittableList(hittable **l, int n) {list = l; listSize = n;}
    
    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override{
        hit_record temp_rec;
        bool hitAnything = false;
        double closestSoFar = t_max;
        for (int i=0; i<listSize; i++) {
            if (list[i]->hit(r, t_min, closestSoFar, temp_rec)) {
                hitAnything = true;
                closestSoFar = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hitAnything;
    }
    
    bool boundingBox(aabb& outputBox) const override {
        if (listSize == 0) return false;
        aabb tempBox;
        bool firstBox = true;
        
        for (int i=0; i<listSize; i++) {
            if (!list[i]->boundingBox(tempBox)) return false;
            outputBox = firstBox ? tempBox : surroundingBox(outputBox, tempBox);
            firstBox = false;
        }
        return true;
    }
    
};
