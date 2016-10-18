#include "shape3d.h"

using namespace std;
using namespace Eigen;

/*
 shape3D::transformShape

 Takes an original shape and a transformation matrix and applies the
 transformation.  It fills an argued shape pointer with the resultant shape.

 Arguments: MatrixXd *matrix - The transformation matrix
            shape3D *transformed - Pointer to the shape to fill with the
                result

 Returns:   Nothing.
 */
void shape3D::transformShape
(
    MatrixXd    *matrix,
    shape3D     *transformed
)
{
    assert(matrix);
    assert(transformed);

    // Ensure we are starting with an empty shape
    transformed->clear();

    // Apply the transformation to every point in the copy
    vector<vertex>::iterator it = this->vertices->begin();
    for (; it != this->vertices->end(); ++it) {
        // Create a vector from the vertex
        MatrixXd vOld(4, 1);
        vOld << it->x, it->y, it->z, 1;

        // Transform the vertex
        MatrixXd vNew = *matrix * vOld;
        vertex v(vNew(0), vNew(1), vNew(2));
        transformed->vertices->push_back(v);
    }

    // The set of faces has the same vertices (only the vertices move)
    vector<facet>::iterator face = this->facets->begin();
    for (; face != this->facets->end(); ++face) {
        transformed->facets->push_back(*face);
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
    // Project every vertex in the shape onto the screen image
    vector<vertex>::iterator v = this->vertices->begin();
    for (; v != this->vertices->end(); ++v) {
        vertex newV;
        v->NDCToImage(xres, yres, cam, &newV);
        outShape->vertices->push_back(newV);
    }

    // Copy all of the facets
    vector<facet>::iterator f = this->facets->begin();
    for (; f != this->facets->end(); ++f) {
        outShape->facets->push_back(*f);
    }
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
    float               near,
    float               far,
    shape3D             *proj
)
{
    assert(proj);

    // Make sure we are using an empty shape
    proj->clear();

    // Use the same name for the projected shape, but append NPC for
    // distinction
    proj->name = this->name + "_NDC";

    // We need to omit facets whose vertices are not in our camera space
    // (z < -far or z > -in).  We will do this by keeping a list of those
    // points that are out of reach, to be referenced when we copy facets
    // over.  No need to delete points, as it is the facets that dictate
    // what is displayed.  We know that this must be in ascending order,
    // which we will use to our advantage when checking vertices for each
    // facet.
    int index;

    // Project each point in the original shape and put it in the new shape
    // Recall that facets assume vertices are 1-indexed, so the 0th vertex
    // is a dummy
    vector<vertex>::iterator v = this->vertices->begin();
    for (; v != this->vertices->end(); ++v) {
        vertex projPt;
        v->worldToCartNDC(toCam, toNDC, &projPt);
        index++;
        proj->vertices->push_back(projPt);
    }
    // All of the facets are still the same set of 3 vertices
    vector<facet>::iterator f = this->facets->begin();
    for (; f != this->facets->end(); ++f) {
        proj->facets->push_back(*f);
    }
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
    std::vector<facet>::iterator f = facets->begin();
    for (; f != facets->end(); ++f) {
        std::cout << "f " << f->v1 << " " << f->v2 << " " << f->v3
                  << std::endl;
    }
    std::cout << std::endl;
}
