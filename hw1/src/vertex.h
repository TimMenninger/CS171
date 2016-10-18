#ifndef VERTEX
#define VERTEX

#include <Eigen/Dense>
#include "camera.h"

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

    _vertex() : x (0), y (0), z (0) {}
    _vertex(float x, float y, float z) : x (x), y (y), z (z) {}
    ~_vertex() {}

    void NDCToImage (int, int, camera*, _vertex*);
    void worldToCartNDC (Eigen::MatrixXd, Eigen::MatrixXd, _vertex*);
} vertex;

#endif // ifndef VERTEX
