#pragma once
#include "vec3.hpp"

class texture {
public:
    virtual vec3 value(double u, double v, const vec3 &point) const = 0;
};

class solidColor : public texture {
    vec3 colorValue;
public:
    solidColor() : colorValue(1,0,1) {}
    solidColor(vec3 color) : colorValue(color) {}
    solidColor(float R, float G, float B) : solidColor(vec3(R,G,B)) {}
    
    virtual vec3 value(double u, double v, const vec3 &point) const override {
        return colorValue;
    }
};

class checkerTexture : public texture {
public:
    texture *odd;
    texture *even;
    
    checkerTexture () : odd(new solidColor()), even(new solidColor()) {}
    checkerTexture (texture *odd, texture *even) : odd(odd), even(even) {}
    checkerTexture (vec3 color1, vec3 color2) : odd(new solidColor(color1)), even(new solidColor(color2)) {}
    
    virtual vec3 value(double u, double v, const vec3 &point) const override {
        auto sines = sin(10*point.x()) * sin(10*point.y()) * sin(10*point.z());
        return sines < 0 ? odd->value(u, v, point) : even->value(u, v, point);
    }
};
