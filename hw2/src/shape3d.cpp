/******************************************************************************

 shape3D.cpp

 Contains methods for manipulating 3 dimensional shapes.

 Author: Tim Menninger

******************************************************************************/
#include "shape3d.h"

using namespace std;
using namespace Eigen;

/*
 shape3D::transformShape

 Applies its transformation matrices to itself

 Returns:   Nothing.
 */
void shape3D::transform
(
)
{
    // Apply the transformation to every point in the copy
    vector<vertex>::iterator v = this->vertices.begin() + 1;
    for (; v != this->vertices.end(); ++v) {
        // Create a vector from the vertex
        MatrixXd vOld(4, 1);
        vOld << v->x, v->y, v->z, 1;

        // Transform the vertex
        MatrixXd vNew = this->ptTransform * vOld;
        *v = vertex(vNew(0), vNew(1), vNew(2));
    }

    // Normals are also all still the same
    vector<normal>::iterator n = this->normals.begin();
    for (; n != this->normals.end(); ++n) {
        // Create a vector from the vertex
        MatrixXd nOld(4, 1);
        nOld << n->x, n->y, n->z, 1;

        // Transform the vertex
        MatrixXd nNew = this->normTransform * nOld;
        *n = normal(nNew(0), nNew(1), nNew(2));
    }
}

/*
 shape3D::NDCToScreen

 Projects a shape whose vertices are in NDC onto a 2D screen.

 Arguments: int xres - X resolution of the image in pixels
            int yres - Y resolution of the image in pixels
            shape3D *out - The projected vertex is stored in the vertices
                attribute of this shape3D

 Returns:   Nothing.
*/
void shape3D::NDCToScreen
(
    int                 xres,
    int                 yres,
    shape3D             *outShape
)
{
    assert(outShape);
    outShape->clear();

    // Keep track of all indices that are not in the camera's view.  Then,
    // We will exclude all facets that have a vertex in this vector
    vector<int> outOfViewPts;
    int index = 1;

    // Corners of the NDC cube a point must be in to be in view.
    point3D crnr1(-1, -1, -1);
    point3D crnr2(1, 1, 1);

    // Project every vertex in the shape onto the screen image
    vector<vertex>::iterator v = this->vertices.begin() + 1;
    for (; v != this->vertices.end(); ++v) {
        vertex newV;
        v->NDCToImage(xres, yres, &newV);

        // We add the vertex regardless of view so that the facets can still
        // index properly
        outShape->vertices.push_back(newV);
        // But we note the index of the invalid vertices
        if (!v->toPoint3D().inCube(crnr1, crnr2)) {
            outOfViewPts.push_back(index);
        }

        index++;
    }

    // Copy all of the facets
    vector<facet>::iterator f = this->facets.begin();
    for (; f != this->facets.end(); ++f) {
        // Only add the facet if all of its vertices are in the view.  Note
        // that because we only add to the vector in one loop, and we only
        // increment each successive value, the vector is sorted ascending
        if (f->inView(outOfViewPts)) {
            outShape->facets.push_back(*f);
        }
    }
    // Normals are also all still the same
    outShape->normals = this->normals;

    // Copy the rest of the shape
    outShape->mat = this->mat;
}

/*
 shape3D::worldToCartNDC

 Takes an object in world space and converts every point to Cartesian NDC,
 preserving the facet vector.

 Arguments: camera cam - Used to get transformation matrices to NDC
            shape3D *proj - Where to store the object projected onto NDC

 Returns:   Nothing.
*/
void shape3D::worldToCartNDC
(
    camera              cam,
    shape3D             *proj
)
{
    assert(proj);

    // Make sure we are using an empty shape
    proj->clear();

    // Use the same name for the projected shape, but append NPC for
    // distinction
    proj->name = this->name + "_NDC";

    // Project each point in the original shape and put it in the new shape
    // Recall that facets assume vertices are 1-indexed, so the 0th vertex
    // is a dummy
    vector<vertex>::iterator v = this->vertices.begin() + 1;
    for (; v != this->vertices.end(); ++v) {
        vertex projPt;
        v->worldToCartNDC(this->ptTransform, cam, &projPt);
        proj->vertices.push_back(projPt);
    }
    // All of the facets are still the same set of 3 vertices
    proj->facets = this->facets;
    // Normals are also all still the same
    proj->normals = this->normals;
    // Copy the rest of the shape
    proj->mat = this->mat;
}
