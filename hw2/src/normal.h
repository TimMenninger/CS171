/******************************************************************************

 normal.h

 Contains normal struct

 Author: Tim Menninger

******************************************************************************/
#ifndef NORMAL
#define NORMAL

#include <Eigen/Dense>

#include "assert.h"

// structs
/*
 normal

 Struct that contains <x, y, z> offsets for a normal vector.
*/
typedef struct _normal {
    float x;
    float y;
    float z;

    _normal() : x (0), y (0), z (0) {}
    _normal(float x, float y, float z) : x (x), y (y), z (z) {}
    ~_normal() {}

} normal;

#endif // ifndef NORMAL
