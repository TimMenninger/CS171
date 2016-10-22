/******************************************************************************

 vertex.h

 Contains vertex struct

 Author: Tim Menninger

******************************************************************************/
#ifndef VERTEX
#define VERTEX

#include <cmath>

#include <Eigen/Dense>
#include "camera.h"
#include "light.h"

// Structs

/*
 vertex

 Struct that defines vertices in 3 (or fewer) dimensions.  Each point is a
 float.
*/
typedef struct _vertex {
    float x;
    float y;
    float z;
    rgb   c;

    _vertex()
        : x (0), y (0), z (0), c(rgb()) {}
    _vertex(float x, float y, float z)
        : x (x), y (y), z (z), c (rgb()) {}
    _vertex(float x, float y, float z, rgb c)
        : x (x), y (y), z (z), c (c) {}
    ~_vertex() {}

    _vertex operator* (const Eigen::MatrixXd& m);

    point3D toPoint3D() { return point3D(x, y, z); }

    void NDCToImage (int, int, _vertex*);
    void worldToCartNDC (Eigen::MatrixXd, Eigen::MatrixXd, Eigen::MatrixXd, _vertex*);
    void worldToCamera (Eigen::MatrixXd, _vertex*);
    void computeLight (normal, rgb, rgb, rgb, float, camera, std::vector<light>);
    float barycentricCoeff (_vertex, _vertex, point3D);
} vertex;

#endif // ifndef VERTEX
