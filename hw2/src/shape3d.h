/******************************************************************************

 shape3D.h

 Contains shape3D struct

 Author: Tim Menninger

******************************************************************************/
#ifndef SHAPE3D
#define SHAPE3D

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
 material

 Defines the light characteristics of the surface of an object
*/
typedef struct _material {
    rgb                 ambient;
    rgb                 diffuse;
    rgb                 specular;
    float               shininess;

    _material() : ambient (rgb()), diffuse (rgb()), specular (rgb()),
        shininess (0) {}
    ~_material() {}
} material;

/*
 shape3D

 Contains a vector of vertices in 3 dimensions and a vector of facets which
 are described by indices in the vertex vector.
*/
typedef struct _shape3D {
    std::string         name;
    std::vector<vertex> vertices;
    std::vector<facet>  facets;
    std::vector<normal> normals;
    material            mat;

    Eigen::MatrixXd     ptTransform;
    Eigen::MatrixXd     normTransform;

    _shape3D() : name (""),
        vertices (), facets (), normals (), mat (material()),
        ptTransform(Eigen::MatrixXd::Identity(4, 4)),
        normTransform(Eigen::MatrixXd::Identity(4, 4)) {}
    ~_shape3D() {}

    void clear() {
        // Clear vectors.  Put dummy value in vertices and normals because
        // they are 1-indexed
        vertices.clear();
        vertices.push_back(vertex());
        normals.clear();
        normals.push_back(normal());
        facets.clear();
    }

    void NDCToScreen (int, int, _shape3D*);
    void worldToCartNDC (camera, _shape3D*);
    void transform ();
} shape3D;


#endif // ifndef SHAPE3D
