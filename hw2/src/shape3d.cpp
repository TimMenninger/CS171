/******************************************************************************

 shape3D.cpp

 Contains methods for manipulating 3 dimensional shapes.

 Author: Tim Menninger

******************************************************************************/
#include "shape3d.h"

using namespace std;
using namespace Eigen;

/*
 shape3D::copy

 Copies a the argued shape into itself.  It does not allocate memory for its
 vectors.

 Arguments: shape3D *toCopy - The shape to copy from

 Returns:   Nothing.
*/
void shape3D::copy
(
    shape3D         *toCopy
)
{
    assert(this->vertices);
    assert(this->facets);
    assert(this->normals);
    assert(toCopy);
    assert(toCopy->vertices);
    assert(toCopy->facets);
    assert(toCopy->normals);

    // Clear the vectors before copying
    this->clear();

    // Copy the material information
    this->ambient = toCopy->ambient;
    this->diffuse = toCopy->diffuse;
    this->specular = toCopy->specular;
    this->shininess = toCopy->shininess;

    // Copy the Transformation matrices
    this->ptTransform = toCopy->ptTransform;
    this->normTransform = toCopy->normTransform;

    // Copy all vertices
    vector<vertex>::iterator v = toCopy->vertices->begin() + 1;
    for (; v != toCopy->vertices->end(); ++v) {
        this->vertices->push_back(*v);
    }

    // Copy all facets
    vector<facet>::iterator f = toCopy->facets->begin();
    for (; f != toCopy->facets->end(); ++f) {
        this->facets->push_back(*f);
    }

    // Copy all normals
    vector<normal>::iterator n = toCopy->normals->begin() + 1;
    for (; n != toCopy->normals->end(); ++n) {
        this->normals->push_back(*n);
    }
}

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
    vector<vertex>::iterator v = this->vertices->begin() + 1;
    for (; v != this->vertices->end(); ++v) {
        // Create a vector from the vertex
        MatrixXd vOld(4, 1);
        vOld << v->x, v->y, v->z, 1;

        // Transform the vertex
        MatrixXd vNew = this->ptTransform * vOld;
        *v = vertex(vNew(0), vNew(1), vNew(2));
    }

    // Normals are also all still the same
    vector<normal>::iterator n = this->normals->begin();
    for (; n != this->normals->end(); ++n) {
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
            camera *cam - The camera view information
            shape3D *out - The projected vertex is stored in the vertices
                attribute of this shape3D

 Returns:   Nothing.
*/
void shape3D::NDCToScreen
(
    int                 xres,
    int                 yres,
    camera              *cam,
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
    vector<vertex>::iterator v = this->vertices->begin() + 1;
    for (; v != this->vertices->end(); ++v) {
        vertex newV;
        v->NDCToImage(xres, yres, &newV);

        // We add the vertex regardless of view so that the facets can still
        // index properly
        outShape->vertices->push_back(newV);
        // But we note the index of the invalid vertices
        if (!v->toPoint3D().inCube(crnr1, crnr2)) {
            outOfViewPts.push_back(index);
        }

        index++;
    }

    // Copy all of the facets
    vector<facet>::iterator f = this->facets->begin();
    for (; f != this->facets->end(); ++f) {
        // Only add the facet if all of its vertices are in the view.  Note
        // that because we only add to the vector in one loop, and we only
        // increment each successive value, the vector is sorted ascending
        if (f->inView(outOfViewPts)) {
            outShape->facets->push_back(*f);
        }
    }
    // Normals are also all still the same
    vector<normal>::iterator n = this->normals->begin();
    for (; n != this->normals->end(); ++n) {
        outShape->normals->push_back(*n);
    }
    // Copy the rest of the shape
    outShape->ambient = this->ambient;
    outShape->diffuse = this->diffuse;
    outShape->specular = this->specular;
    outShape->shininess = this->shininess;
}

/*
 shape3D::worldToCartNDC

 Takes an object in world space and converts every point to Cartesian NDC,
 preserving the facet vector.

 Arguments: MatrixXd toCam - Transformation matrix from world space to camera
                space
            MatrixXd toNDC - Transformation matrix from camera space to
                NDC
            float near - The near parameter of the camera view
            float far - The far parameter (or frustum) of the camera space
            shape3D *proj - Where to store the object projected onto NDC

 Returns:   Nothing.
*/
void shape3D::worldToCartNDC
(
    MatrixXd            toCam,
    MatrixXd            toNDC,
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
    vector<vertex>::iterator v = this->vertices->begin() + 1;
    for (; v != this->vertices->end(); ++v) {
        vertex projPt;
        v->worldToCartNDC(this->ptTransform, toCam, toNDC, &projPt);
        proj->vertices->push_back(projPt);
    }
    // All of the facets are still the same set of 3 vertices
    vector<facet>::iterator f = this->facets->begin();
    for (; f != this->facets->end(); ++f) {
        proj->facets->push_back(*f);
    }
    // Normals are also all still the same
    vector<normal>::iterator n = this->normals->begin();
    for (; n != this->normals->end(); ++n) {
        proj->normals->push_back(*n);
    }
    // Copy the rest of the shape
    proj->ambient = this->ambient;
    proj->diffuse = this->diffuse;
    proj->specular = this->specular;
    proj->shininess = this->shininess;
}

/*
 shape3D::worldToCamera

 Takes a shape and transforms it into camera space.

 Arguments: MatrixXd toCam - The matrix to convert the shape to camera space.
            shape3D *proj - The new shape is put in here.

 Returns:   Nothing.
*/
void shape3D::worldToCamera
(
    MatrixXd            toCam,
    shape3D             *proj
)
{
    assert(proj);

    // Make sure we are using an empty shape
    proj->clear();

    // Use the same name for the projected shape, but append NPC for
    // distinction
    proj->name = this->name;

    // Project each point in the original shape and put it in the new shape
    // Recall that facets assume vertices are 1-indexed, so the 0th vertex
    // is a dummy
    vector<vertex>::iterator v = this->vertices->begin() + 1;
    for (; v != this->vertices->end(); ++v) {
        vertex projPt;
        v->worldToCamera(toCam, &projPt);
        proj->vertices->push_back(projPt);
    }
    // All of the facets are still the same set of 3 vertices
    vector<facet>::iterator f = this->facets->begin();
    for (; f != this->facets->end(); ++f) {
        proj->facets->push_back(*f);
    }
    // Normals are transformed by inverse transpose of camera matrix
    vector<normal>::iterator n = this->normals->begin();
    for (; n != this->normals->end(); ++n) {
        proj->normals->push_back(*n);
    }
    // Copy the rest of the shape
    proj->ambient = this->ambient;
    proj->diffuse = this->diffuse;
    proj->specular = this->specular;
    proj->shininess = this->shininess;
}

void shape3D::print()
{
    // Can't have two threads writing to file
    std::cout << name << ":" << std::endl << std::endl;
    std::vector<vertex>::iterator v = vertices->begin() + 1;
    for (; v != vertices->end(); ++v) {
        std::cout << "v " << v->x << " " << v->y << " " << v->z
                  << std::endl;
    }
    vector<normal>::iterator vn = this->normals->begin();
    for (; vn != this->normals->end(); ++vn) {
        std::cout << "vn " << vn->x << " " << vn->y << " " << vn->z
                  << std::endl;
    }
    std::vector<facet>::iterator f = facets->begin();
    for (; f != facets->end(); ++f) {
        std::cout << "f " << f->v1 << " " << f->v2 << " " << f->v3
                  << std::endl;
    }
    std::cout << std::endl;
}
