#ifndef CAMERA
#define CAMERA

#include "geom.h"

// Structs
/*
 orientation

 Orientation is defined as a point in 3d space and an angle in radians.
*/
typedef struct _orientation {
    point3D vec;
    float theta;

    _orientation() : vec (point3D()), theta (0) {}
    _orientation(float x, float y, float z, float t) :
        vec (point3D(x, y, z)), theta (t) {}
    ~_orientation() {}
} orientation;

/*
 camera

 A camera has several values that distinguish where it is in space, and what
 exacty it can "see".
*/
typedef struct _camera {
    point3D pos;
    orientation orient;
    float near;
    float far;
    float left;
    float right;
    float top;
    float bottom;

    _camera() :
        pos (point3D()),
        orient (orientation()),
        near (0),
        far (0),
        left (0),
        right (0),
        top (0),
        bottom(0)
        {}
    _camera(float px, float py, float pz, float ox, float oy, float oz, float t,
            float near, float far, float lt, float rt, float top, float bot) :
        pos (point3D(px, py, pz)),
        orient (orientation(ox, oy, oz, t)),
        near (near),
        far (far),
        left (lt),
        right (rt),
        top (top),
        bottom (bot)
        {}
    ~_camera() {}
} camera;

#endif // ifndef CAMERA
