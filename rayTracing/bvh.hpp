#pragma once
#include <stdio.h>
#include "hittable.hpp"
#include "hittableList.hpp"
#include "random.hpp"

inline bool box_compare(const hittable *a, const hittable *b, int axis) {
    aabb boxA, boxB;
    if (!a->boundingBox(boxA) or !b->boundingBox(boxB)) std::cerr << "Object without bounding box is compared\n";
    return boxA.min().e[axis] < boxB.min().e[axis];
}

inline bool box_x_compare(const hittable *a, const hittable *b) {return box_compare(a, b, 0);}
inline bool box_y_compare(const hittable *a, const hittable *b) {return box_compare(a, b, 1);}
inline bool box_z_compare(const hittable *a, const hittable *b) {return box_compare(a, b, 2);}

class bvhNode : public hittable {
    hittable *left;
    hittable *right;
    aabb box;
    
public:
    bvhNode();
    bvhNode(const hittableList &list) : bvhNode(list.list, list.listSize, 0, list.listSize) {}
    bvhNode(hittable **objects, size_t listSize, size_t start, size_t end) {
        int axis = randomInt(0, 2); // choose a random axis to split
        auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;
        size_t objectSpan = end - start;
        
        if (objectSpan == 1) {
            left = right = objects[start];
        } else if (objectSpan == 2) {
            if (comparator(objects[start], objects[start+1])) {
                left = objects[start];
                right = objects[start+1];
            } else {
                left = objects[start+1];
                right = objects[start];
            }
        } else { // more than 2 objects left in the list
            std::sort(objects+start, objects+end, comparator);
            auto middlePoint = start + objectSpan / 2;
            left = new bvhNode(objects, listSize, start, middlePoint);
            right = new bvhNode(objects, listSize, middlePoint, end);
        }
        aabb boxLeft, boxRight;
        if (!left->boundingBox(boxLeft) or !right->boundingBox(boxRight)) {
            std::cerr << "Object without bounding box is sent to constructing bvhNode\n";
        }
        box = surroundingBox(boxLeft, boxRight);
    }
    
    bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const override{
        if (!box.hit(r, t_min, t_max)) return false;
        bool hitLeft = left->hit(r, t_min, t_max, rec);
        bool hitRight = right->hit(r, t_min, hitLeft ? rec.t : t_max, rec);
        return (hitLeft || hitRight);
    }
    
    bool boundingBox(aabb& outputBox) const override {
        outputBox = box;
        return true;
    }
};
