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
 worldToCameraMatrix

 Creates a transformation matrix for projecting points in world space onto
 camera space.

 Arguments: camera cam - The camera information describing the view point
            MatrixXd *transform - The output matrix that will contain the
                transformation matrix.

 Returns:   Nothing.
*/
void worldToCameraMatrix
(
    camera              cam,
    MatrixXd            *transform
)
{
    assert(transform);

    // Make translation matrix for all objects using camera position as vector
    ostringstream os;
    os << "t "
       << cam.pos.x << " "
       << cam.pos.y << " "
       << cam.pos.z;
    string vec = os.str();
    MatrixXd Tc(4, 4);
    generateTransform(vec, &Tc);

    // Make rotation matrix for all objects using camera orientation
    os.clear();
    os << "r "
       << cam.orient.vec.x << " "
       << cam.orient.vec.y << " "
       << cam.orient.vec.z << " "
       << cam.orient.theta;
    vec = os.str();
    MatrixXd Rc(4, 4);
    generateTransform(vec, &Rc);

    // World to camera matrix is simply the product of the translation matrix
    // and the rotation matrix
    *transform = (Tc * Rc).inverse();
}

/*
 perspectiveProjectionMatrix

 Obtained from course website:
 http://courses.cms.caltech.edu/cs171/assignments/hw1/hw1-notes/notes-hw1.html
 The perspective projection matrix is given by

          2*n/(r-l)           0       (r+l)/(r-l)       0
              0           2*n/(t-b)   (t+b)/(t-b)       0
              0               0      -(f+n)/(f-n)  -2*f*n/(f-n)
              0               0           -1            0

 where n, f, r, l, t, and b are the near, far, right, left, top and bottom
 attributes of the camera struct, respectively.

 Arguments: camera cam - Contains information about the camera or eye
            MatrixXd *transform - The matrix that will be populated by this
                function.

 Returns:   Nothing.
*/
void perspectiveProjectionMatrix
(
    camera              cam,
    MatrixXd            *transform
)
{
    assert(transform);

    float n = cam.near;
    float f = cam.far;
    float r = cam.right;
    float l = cam.left;
    float t = cam.top;
    float b = cam.bottom;

    // Generate the matrix and populate what is pointed to in the arguments
    *transform << 2*n/(r-l),         0,  (r+l)/(r-l),            0,
                          0, 2*n/(t-b),  (t+b)/(t-b),            0,
                          0,         0, -(f+n)/(f-n), -2*f*n/(f-n),
                          0,         0,           -1,            0;
}

/*
 worldToCartNDC

 Takes a "world" of shapes (stored in vectors) all in world space and
 projects them all onto NDC.

 Arguments: camera cam - The camera view parameters
            map<string, vector<shape3D> > shapes - A map that maps names of
                original objects to vectors of transformed copies of them.
                The original shape is not in the vector.
            vector<shape3D> *NDCShapes - An output vector of all of the shapes
                from the map, projected onto NDC

 Returns:   Nothing.
*/
void worldToCartNDC
(
    camera                          cam,
    map<string, vector<shape3D> >   shapes,
    vector<shape3D>                 *NDCShapes
)
{
    // First get the transformation matrix that can convert world space to
    // camera space
    MatrixXd toCam(4, 4);
    worldToCameraMatrix(cam, &toCam);

    // Get the perspective projection matrix that we will use to get from a
    // point in camera space to our screen
    MatrixXd toNDC(4, 4);
    perspectiveProjectionMatrix(cam, &toNDC);

    // The shapes are organized in vectors indexed by the original shape they
    // copies, where each vector contains all the different transformations
    // done to that shape
    map<string, vector<shape3D> >::iterator copySet = shapes.begin();
    for (; copySet != shapes.end(); ++copySet) {
        vector<shape3D>::iterator s = copySet->second.begin();
        for (; s != copySet->second.end(); ++s) {
            shape3D proj;
            proj.vertices = new vector<vertex>;
            proj.facets = new vector<facet>;

            // Convert the shape to Cartesian NDC, removing facets not in
            // camera space
            s->worldToCartNDC(toCam, toNDC, cam.near, cam.far, &proj);
            NDCShapes->push_back(proj);
        }
    }
}
