#include "vertex.h"

using namespace std;
using namespace Eigen;

/*
 vertex::worldToCartNDC

 Takes a point and a perspective projection matrix and converts the original
 point to Cartesian normalized device coordinates (NDC).

 Arguments: MatrixXd toCam - The transformation matrix to bring a point from
                world space to camera space
            MatrixXd toNDC - The transformation matrix to bring a point from
                camera space to NDC
            vertex *proj - Where to store the projected point

 Returns:   Nothing.
*/
void vertex::worldToCartNDC
(
    MatrixXd            toCam,
    MatrixXd            toNDC,
    vertex              *proj
)
{
    assert(proj);

    // Create a 4D vector (x, y, z, w) out of the original point where w = 1
    MatrixXd origVec(4, 1);
    origVec << this->x, this->y, this->z, 1;

    // The output homogenous NDC point will be a 4D vector, and is computed
    // by multiplying the perspective projection matrix by the original point
    MatrixXd NDC = toNDC * (toCam * origVec);

    // The cartesian point is the x, y, z coordinates of the homogenous NDC
    // normalized by scaling by -1/w, where w is at the 3rd index (0-indexed)
    NDC /= -1 * NDC(3);

    // Use the resultant 4D vector to populate the argued address with the
    // projected point
    *proj = vertex(NDC(0), NDC(1), NDC(2));
}

/*
 vertex::NDCToImage

 Projects a vertex in NDC onto the 2D screen from which we are viewing.

 Arguments: int xres - X resolution of the image in pixels
            int yres - Y resolution of the image in pixels
            camera *cam - The camera view information
            vertex *out - The projected vertex is stored in this vertex

 Returns:   Nothing.
*/
void vertex::NDCToImage
(
    int                 xres,
    int                 yres,
    camera              *cam,
    vertex              *out
)
{
    // Only want to add this point if it is within (-1, -1, -1) and (1, 1, 1)
    float x = this->x, y = this->y, z = this->z;
    if (x > 1 || y > 1 || x < -1 || y < -1)
        return;

    // Point is in view, scale NDC (-1, 1) to (0, xres) and (0, yres) and add
    // it to the image
    int winX = (1 + x) * (xres / 2);
    int winY = ((1 + y) * (yres / 2));
    *out = vertex(winX, winY, 0);
}
