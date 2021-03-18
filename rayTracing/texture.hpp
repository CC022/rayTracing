#pragma once
#include <fstream>
#include <string>
#include <algorithm>
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

class imageTexture : public texture {
    uint8_t *imageBuffer = nullptr;
    int h, w, max;
    
public:
    imageTexture(std::string ppmFile) {
        std::ifstream ppmFileStream(ppmFile);
        if (!ppmFileStream.is_open()) {throw std::runtime_error("unable to open ppm file: " + ppmFile);}
        std::string ppmMagic;
        ppmFileStream >> ppmMagic >> w >> h >> max;
        ppmFileStream.seekg(1, ppmFileStream.cur);
        imageBuffer = new uint8_t[h*w*3];
        ppmFileStream.read(reinterpret_cast<char*>(imageBuffer), w*h*3);
    }
    
    ~imageTexture() {
        delete [] imageBuffer;
    }
    
    virtual vec3 value(double u, double v, const vec3 &point) const override {
        if (imageBuffer == nullptr) {return vec3(1, 0, 1);}
        u = std::clamp(u, 0.0, 1.0);
        v = 1.0 - std::clamp(v, 0.0, 1.0); // Flip v to image coordinates
        auto i = static_cast<int>(u * w);
        auto j = static_cast<int>(v * h);
        i = std::clamp(i, 0, w-1);
        j = std::clamp(j, 0, h-1);
        const float colorScale = 1.0 / max;
        auto pixelBase = j*w*3 + i*3;
        return vec3(colorScale*imageBuffer[pixelBase], colorScale*imageBuffer[pixelBase+1], colorScale*imageBuffer[pixelBase+2]);
    }
    
};
