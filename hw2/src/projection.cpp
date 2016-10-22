/******************************************************************************

 projection.cpp

 Projects an object from world space to camera space and to normalized device
 coordinates

 Author: Tim Menninger

******************************************************************************/
#include "projection.h"

using namespace std;
using namespace Eigen;

/*
 worldToCartNDC

 Takes a "world" of shapes (stored in vectors) all in world space and
 projects them all onto NDC.

 Arguments: camera cam - The camera view parameters
            vector<shape3D> *shapes - A vector of original shapes that
                will be transformed.
            vector<shape3D> *NDCShapes - An output vector of all of the shapes
                from the map, projected onto NDC

 Returns:   Nothing.
*/
void worldToCartNDC
(
    camera                          cam,
    vector<shape3D>                 *shapes,
    vector<shape3D>                 *NDCShapes
)
{
    // Convert each shape to NDC
    vector<shape3D>::iterator s = shapes->begin();
    for (; s != shapes->end(); ++s) {
        shape3D proj;

        // Convert the shape to Cartesian NDC, removing facets not in
        // camera space
        s->worldToCartNDC(cam, &proj);
        NDCShapes->push_back(proj);
    }
}
