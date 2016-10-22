/******************************************************************************

 light.h

 Contains light struct and the rgb color struct

 Author: Tim Menninger

******************************************************************************/
#ifndef LIGHT
#define LIGHT

#include <math.h>
#include <vector>
#include <stdint.h>

#include "utils.h"
#include "geom.h"
#include "transform.h"
#include "geom.h"
#include "camera.h"
#include "normal.h"

// structs
/*
 rgb

 Red/green/blue values of a color of a pixel.  This assumes the maximum
 intensity is 1.
*/
typedef struct _rgb {
    float r;
    float g;
    float b;
    float a;

    _rgb() : r (0), g (0), b (0), a (1) {}
    _rgb(float r, float g, float b) : r (r), g (g), b (b), a(1) {}
    _rgb(float r, float g, float b, float a) : r (r), g (g), b (b), a(a) {}
    ~_rgb() {}

    _rgb operator*(const float& k) { return _rgb(k*r, k*g, k*b); };
    _rgb operator+(const _rgb& c) { return _rgb(c.r+r, c.g+g, c.b+b); }

    // Undefined behavior if intensity > 255, but will not crash program
    uint32_t toUInt32(int intensity) {
        uint32_t color = 0;
        float *vals[4] = {&r, &g, &b, &a};
        for (int i = 0; i < 4; ++i) {
            color <<= 8;
            color += (int) (*vals[i] * intensity);
        }
        return color;
    }
} rgb;

/*
 light

 Defines a light source.
*/
typedef struct _light {
    point3D     loc;
    rgb         color;
    float       attenuation;

    _light() : loc (point3D()), color (rgb()), attenuation (0) {}
    _light(point3D l, rgb c, float a) : loc (l), color (c), attenuation (a) {}
    ~_light() {}
} light;

#endif // ifndef LIGHT
