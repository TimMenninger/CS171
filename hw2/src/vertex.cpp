/******************************************************************************

 vertex.cpp

 Contains methods for manipulating vertices.

 Author: Tim Menninger

******************************************************************************/
#include "vertex.h"

using namespace std;
using namespace Eigen;

/*
 vertex::operator*

 Defines the multiplication operator for a vertex and a matrix.  It creates a
 matrix out of the vertex, multiplies by the matrix, and returns the vertex
 with the resultant matrix values.

 Arguments: const MatrixXd &m - The matrix to multiply by

 Returns:   (vertex) - Vertex after multiplication
*/
vertex vertex::operator*
(
    const MatrixXd      &m
)
{
    // Want to handle 3D and 4D
    int dim = m.rows();
    MatrixXd vec(dim, 1);

    // Populate the vector to match the input matrix's dimension
    switch(dim) {
        case 3:
            vec << this->x, this->y, this->z;
            break;
        case 4:
            vec << this->x, this->y, this->z, 1;
            break;
        default:
            return vertex();
    }

    // Do the computation and turn it back to a vertex
    vec = m * vec;

    return vertex(vec(0), vec(1), vec(2));
}

/*
 vertex::worldToCartNDC

 Takes a point and a perspective projection matrix and converts the original
 point to Cartesian normalized device coordinates (NDC).

 Arguments: MatrixXd transform - The transformation matrix to apply
            camera cam - Used to convert to camera space then NDC
            vertex *proj - Where to store the projected point

 Returns:   Nothing.
*/
void vertex::worldToCartNDC
(
    MatrixXd            transform,
    camera              cam,
    vertex              *proj
)
{
    assert(proj);

    // First get the transformation matrix that can convert world space to
    // camera space
    MatrixXd toCam(4, 4);
    cam.worldToCameraMatrix(&toCam);

    // Get the perspective projection matrix that we will use to get from a
    // point in camera space to our screen
    MatrixXd toNDC(4, 4);
    cam.perspectiveProjectionMatrix(&toNDC);

    // Create a 4D vector (x, y, z, w) out of the original point where w = 1
    MatrixXd origVec(4, 1);
    origVec << this->x, this->y, this->z, 1;

    // The output homogenous NDC point will be a 4D vector, and is computed
    // by multiplying the perspective projection matrix by the original point
    MatrixXd NDC = toNDC * (toCam * (transform * origVec));

    // The cartesian point is the x, y, z coordinates of the homogenous NDC
    // normalized by scaling by 1/w, where w is at the 3rd index (0-indexed)
    NDC /= NDC(3);

    // Use the resultant 4D vector to populate the argued address with the
    // projected point
    *proj = vertex(NDC(0), NDC(1), NDC(2));
}

/*
 vertex::NDCToImage

 Projects a vertex in NDC onto the 2D screen from which we are viewing.

 Arguments: int xres - X resolution of the image in pixels
            int yres - Y resolution of the image in pixels
            vertex *out - The projected vertex is stored in this vertex

 Returns:   Nothing.
*/
void vertex::NDCToImage
(
    int                 xres,
    int                 yres,
    vertex              *out
)
{
    float x = this->x, y = this->y;
    // Point is in view, scale NDC (-1, 1) to (0, xres) and (0, yres) and add
    // it to the image
    int winX = (1 + x) * (xres / 2);
    int winY = (1 + y) * (yres / 2);
    *out = vertex(winX, winY, 0);
}

/*
 vertex::computeLight

 Computes the lighting of this vertex given all of the light sources and their
 parameters in the system.

 Arguments: normal n - The normal to this vertex.
            rgb ambient - Ambient RGB value of this shape
            rgb diffuse - Diffuse RGB value of this shape
            rgb specular - Specular RGB value of this shape
            float shininess - Shininess of the shape
            camera cam - The camera the image is viewed from
            vector<light> lights - Lights in the system

 Returns:   Nothing.
*/
void vertex::computeLight
(
    normal          n,
    rgb             ambient,
    rgb             diffuse,
    rgb             specular,
    float           shininess,
    camera          cam,
    vector<light>   lights
)
{
    // Accumulators for computing light intensity
    rgb diffuseSum(0, 0, 0);
    rgb specularSum(0, 0, 0);

    // Vector at point P in direction of camera
    point3D eDir(cam.pos.x - this->x,
        cam.pos.y - this->y, cam.pos.z - this->z);
    eDir.normalize();

    // Compute the aggregate light contribution of the lights on point v
    vector<light>::iterator l = lights.begin();
    for (; l != lights.end(); ++l) {
        // Compute the distance so we can get attenuation
        float dist = l->loc.distanceTo(this->toPoint3D());
        // Get the attenuation constant from the light source
        float k = l->attenuation;
        float attenuation = 1 / (1 + k*dist*dist);

        // Scale light intensity by attenuation
        l->color = l->color * attenuation;

        // Vector in direction of light source
        point3D lDir(l->loc.x - this->x,
            l->loc.y - this->y, l->loc.z - this->z);
        lDir.normalize();

        // Point representation of normal
        point3D normPt(n.x, n.y, n.z);
        // Add in to diffuse
        float colorScale = lDir.dot(normPt);
        colorScale = colorScale < 0 ? 0 : colorScale;
        diffuseSum = diffuseSum + (l->color * colorScale);
        // Add light to specular
        point3D eyePlusLight = eDir + lDir;
        eyePlusLight.normalize();
        float specScale = eyePlusLight.dot(normPt);
        specScale = specScale < 0 ? 0 : specScale;
        specularSum = specularSum + (l->color * pow(specScale, shininess));
    }

    // Using aggregate light, determine intensity of r, g and b
    float r = ambient.r + diffuseSum.r * diffuse.r +
        specularSum.r * specular.r;
    float g = ambient.g + diffuseSum.g * diffuse.g +
        specularSum.g * specular.g;
    float b = ambient.b + diffuseSum.b * diffuse.b +
        specularSum.b * specular.b;

    // Can't have intensities greater than 1
    this->c = rgb(r > 1 ? 1 : r, g > 1 ? 1 : g, b > 1 ? 1 : b);
}

/*
 vertex::barycentricCoeff

 Returns the barycentric coefficient for this vertex given the other two
 vertices on the same facet and the pixel, which is assumed to be inside
 the facet.

 Arguments: vertex pt1 - Point on the facet
            vertex pt2 - Other point on the facet
            point3D pixel - The point that the coefficient will be used to
                calculate for

 Returns:   (float) - The barycentric coefficient
*/
float vertex::barycentricCoeff
(
    vertex          pt1,
    vertex          pt2,
    point3D         pixel
)
{
    return ((pt1.y - pt2.y) * pixel.x + (pt2.x - pt1.x) * pixel.y +
                pt1.x * pt2.y - pt1.y * pt2.x)
         / ((pt1.y - pt2.y) * this->x + (pt2.x - pt1.x) * this->y +
                pt1.x * pt2.y - pt1.y * pt2.x);
}
