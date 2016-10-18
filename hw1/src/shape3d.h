#ifndef SHAPE3D
#define SHAPE3D

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>

#include <Eigen/Dense>

#include "geom.h"
#include "vertex.h"
#include "facet.h"
#include "camera.h"


// Structs
/*
 shape3D

 Contains a vector of vertices in 3 dimensions and a vector of facets which
 are described by indices in the vertex vector.
*/
typedef struct _shape3D {
    std::string         name;
    std::vector<vertex> *vertices;
    std::vector<facet>  *facets;

    _shape3D() : name (""), vertices (NULL), facets (NULL) {}
    _shape3D(std::string name, std::vector<vertex> *v, std::vector<facet> *f) :
                name (name), vertices (v), facets (f) {}
    ~_shape3D() {}

    void clear() {
        vertices->clear();
        facets->clear();
    }

    void NDCToScreen (int, int, camera*, _shape3D*);
    void worldToCartNDC (Eigen::MatrixXd, Eigen::MatrixXd, float, float, _shape3D*);
    void transformShape (Eigen::MatrixXd*, _shape3D*);

    void print();
} shape3D;


#endif // ifndef SHAPE3D
