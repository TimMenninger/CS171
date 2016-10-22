/******************************************************************************

 shape3D.h

 Contains shape3D struct

 Author: Tim Menninger

******************************************************************************/
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
#include "normal.h"
#include "light.h"


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
    std::vector<normal> *normals;
    rgb                 ambient;
    rgb                 diffuse;
    rgb                 specular;
    float               shininess;

    Eigen::MatrixXd     ptTransform;
    Eigen::MatrixXd     normTransform;

    _shape3D() : name (""), vertices (NULL), facets (NULL), normals (NULL),
        ambient (rgb()), diffuse (rgb()), specular (rgb()), shininess (0),
        ptTransform(Eigen::MatrixXd::Identity(4, 4)),
        normTransform(Eigen::MatrixXd::Identity(4, 4)) {}
    ~_shape3D() {}

    void clear() {
        // Clear vectors.  Put dummy value in vertices and normals because
        // they are 1-indexed
        vertices->clear();
        vertices->push_back(vertex());
        normals->clear();
        normals->push_back(normal());
        facets->clear();
    }

    void NDCToScreen (int, int, camera*, _shape3D*);
    void worldToCartNDC (Eigen::MatrixXd, Eigen::MatrixXd, _shape3D*);
    void worldToCamera (Eigen::MatrixXd, _shape3D*);
    void transform ();

    void copy(_shape3D*);
    void print();
} shape3D;


#endif // ifndef SHAPE3D
