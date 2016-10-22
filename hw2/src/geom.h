/******************************************************************************

 geom.h

 Contains structs for general geometry.

 Author: Tim Menninger

******************************************************************************/
#ifndef GEOM
#define GEOM

#include <math.h>

/*
 point2D

 A 2D point on a plane.
*/
typedef struct _point2D {
    float       x;
    float       y;

    _point2D() : x (0), y (0) {}
    _point2D(float x, float y) : x (x), y (y) {}
    ~_point2D() {}
} point2D;

/*
 point3D

 A three-dimensional point.
*/
typedef struct _point3D {
    float x;
    float y;
    float z;

    _point3D() : x (0), y (0), z (0) {}
    _point3D(float x, float y, float z) : x (x), y (y), z (z) {}
    ~_point3D() {}

    void normalize() {
        float n = sqrt(x*x + y*y + z*z);
        *this = _point3D(x/n, y/n, z/n);
    }
    float dot(_point3D pt) { return x*pt.x + y*pt.y + z*pt.z; }
    _point3D operator+ (const _point3D& p) {
        return _point3D(p.x+x, p.y+y, p.z+z);
    }
    float distanceTo(_point3D p) {
        return sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z));
    }
    bool inCube(_point3D p1, _point3D p2) {
        return (x >= p1.x && y >= p1.y && z >= p1.z &&
                x <= p2.x && y <= p2.y && z <= p2.z)
            || (x >= p2.x && y >= p2.y && z >= p2.z &&
                x <= p1.x && y <= p1.y && z <= p1.z);
    }

} point3D;

#endif // ifndef GEOM
