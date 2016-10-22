/******************************************************************************

 camera.cpp

 Contains methods for creating matrices based on the camera

 Author: Tim Menninger

******************************************************************************/
#include "camera.h"

using namespace Eigen;
using namespace std;

/*
 camera::worldToCameraMatrix

 Creates a transformation matrix for projecting points in world space onto
 camera space.

 Arguments: MatrixXd *transform - The output matrix that will contain the
                transformation matrix.

 Returns:   Nothing.
*/
void camera::worldToCameraMatrix
(
    MatrixXd            *transform
)
{
    assert(transform);

    // Make translation matrix for all objects using camera position as vector
    ostringstream os;
    os << "t "
       << this->pos.x << " "
       << this->pos.y << " "
       << this->pos.z;
    string vec = os.str();
    MatrixXd Tc(4, 4);
    generateTransform(vec, &Tc);

    // Make rotation matrix for all objects using camera orientation
    os.clear();
    os << "r "
       << this->orient.vec.x << " "
       << this->orient.vec.y << " "
       << this->orient.vec.z << " "
       << this->orient.theta;
    vec = os.str();
    MatrixXd Rc(4, 4);
    generateTransform(vec, &Rc);

    // World to camera matrix is simply the product of the translation matrix
    // and the rotation matrix
    *transform = (Tc * Rc).inverse();
}

/*
 camera::perspectiveProjectionMatrix

 Obtained from course website:
 http://courses.cms.caltech.edu/cs171/assignments/hw1/hw1-notes/notes-hw1.html
 The perspective projection matrix is given by

          2*n/(r-l)           0       (r+l)/(r-l)       0
              0           2*n/(t-b)   (t+b)/(t-b)       0
              0               0      -(f+n)/(f-n)  -2*f*n/(f-n)
              0               0           -1            0

 where n, f, r, l, t, and b are the near, far, right, left, top and bottom
 attributes of the camera struct, respectively.

 Arguments: MatrixXd *transform - The matrix that will be populated by this
                function.

 Returns:   Nothing.
*/
void camera::perspectiveProjectionMatrix
(
    MatrixXd            *transform
)
{
    assert(transform);

    float n = this->near;
    float f = this->far;
    float r = this->right;
    float l = this->left;
    float t = this->top;
    float b = this->bottom;

    // Generate the matrix and populate what is pointed to in the arguments
    *transform << 2*n/(r-l),         0,  (r+l)/(r-l),            0,
                          0, 2*n/(t-b),  (t+b)/(t-b),            0,
                          0,         0, -(f+n)/(f-n), -2*f*n/(f-n),
                          0,         0,           -1,            0;
}
