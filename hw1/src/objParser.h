#ifndef OBJPARSER
#define OBJPARSER

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>

#include "geom.h"


#define FEXT_LEN        4         // Number of characters in OBJ file extension


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
} vertex;

/*
 facet

 Struct that defines three vertices that describe the face.  Each vertex is
 an integer index to a list of vertices.
*/
typedef struct _facet {
    int v1;
    int v2;
    int v3;

    _facet() : v1 (0), v2 (0), v3 (0) {}
    _facet(int v1, int v2, int v3) : v1 (v1), v2 (v2), v3 (v3) {}
    ~_facet() {}
} facet;

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

    void print(){
        // Can't have two threads writing to file
        std::cout << name << ":" << std::endl << std::endl;
        std::vector<vertex>::iterator v = vertices->begin() + 1;
        for (; v != vertices->end(); ++v) {
            std::cout << "v " << v->x << " " << v->y << " " << v->z
                      << std::endl;
        }
        std::vector<facet>::iterator f = facets->begin();
        for (; f != facets->end(); ++f) {
            std::cout << "f " << f->v1 << " " << f->v2 << " " << f->v3
                      << std::endl;
        }
        std::cout << std::endl;}
} shape3D;


// Externally public functions
int parseObjFile (char*, shape3D*);
int parseObjFile (std::string, shape3D*);

#endif // ifndef OBJPARSER
