#ifndef GEOM
#define GEOM

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
} point3D;

#endif // ifndef GEOM
