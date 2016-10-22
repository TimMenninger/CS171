/******************************************************************************

 image.cpp

 Contains methods that convert objects whose points are in NDC to be viewable
 on a 2D screen.

 Author: Tim Menninger

******************************************************************************/
#include "image.h"

using namespace Eigen;
using namespace std;

/*
 facingCamera

 This takes three vertices and constructs two vectors, with both tails at
 v2.  It computes the z coordinate of the cross product of these vectors.  If
 it is positive, then the facet described by the three argued vertices is not
 facing the camera, and false is returned.  True is returned otherwise.

 Arguments: vertex v1 - The first vertex
            vertex v2 - The second vertex.  Both vectors' tail
            vertex v3 - The third vertex

 Returns:   (bool) - True if the facet described by these vertices is facing
                the camera
*/
bool facingCamera
(
    vertex          v1,
    vertex          v2,
    vertex          v3
)
{
    // Only concerned about the z component of (v3 - v2) X (v1 - v2), which
    // can be gotten from only the x and y coordinates of each.
    point2D v3mv2(v3.x - v2.x, v3.y - v2.y);
    point2D v1mv2(v1.x - v2.x, v1.y - v2.y);

    // Crossed z is just determinant of that 2D matrix [[x1 y1] [x2 y2]]
    return (v3mv2.x * v1mv2.y - v3mv2.y * v1mv2.x) > 0;
}

/*
 NDCToScreen

 Projects a vector of shapes whose vertices are all in NDC onto a 2D screen,
 saving the projected objects in a supplied vector.

 Arguments: int xres - X resolution of the image in pixels
            int yres - Y resolution of the image in pixels
            camera cam - The camera view information
            vector<shape3D> NDCShapes - Vector of shapes whose vertices are
                in NDC
            vector<shape3D> *screenShapes - Where to store the screen
                projected shapes

 Returns:   Nothing.
*/
void NDCToScreen
(
    int                 xres,
    int                 yres,
    camera              *cam,
    vector<shape3D>     NDCShapes,
    vector<shape3D>     *screenShapes
)
{
    screenShapes->clear();

    // Project every shape onto the screen in 2D
    vector<shape3D>::iterator shape = NDCShapes.begin();
    for (; shape != NDCShapes.end(); ++shape) {
        shape3D scrShape;
        scrShape.vertices = new vector<vertex>;
        scrShape.facets = new vector<facet>;
        scrShape.normals = new vector<normal>;
        shape->NDCToScreen(xres, yres, cam, &scrShape);
        screenShapes->push_back(scrShape);
    }
}

/*
 image::drawLine

 This uses Bresenham's algorithm to draw a line on an image.

 Arguments: point2D p0 - One endpoint of the line
            point2D p1 - Other endpoint of the line
            uint32_t color - The RGBA color that the line will be on the image

 Returns:   Nothing.
*/
void image::drawLine
(
    point2D             p0,
    point2D             p1,
    uint32_t            color
)
{
    // Always putting point with larger x into point 0 allows us to only worry
    // about 4 octants
    float e = 0;
    float x0 = (p0.x > p1.x ? p1.x : p0.x);
    float x1 = (p0.x > p1.x ? p0.x : p1.x);
    float y0 = (p0.x > p1.x ? p1.y : p0.y);
    float y1 = (p0.x > p1.x ? p0.y : p1.y);
    float dx = x1 - x0; // Note: always positive
    float dy = y1 - y0;
    float m = dy / dx;  // Slope of the line

    // Use this so we don't make the same comparison a bunch of times
    bool dxGreater = fabs(dx) >= fabs(dy);

    // x takes on the lesser of x0 and x1 iff. slope < -1
    // y takes on greater of y0 and y1 iff. -1 <= slope <= 0
    // Note that y1 > y0 iff. the slope is positive, leaving us with same
    // condition as for x
    int x = (int) (m < -1 ? x1 : x0);
    int y = (int) (m < -1 ? y1 : y0);

    // We iterate over the dimension with the longer delta
    int *i = dxGreater ? &x : &y;
    float iBnd = dxGreater ? x1 : max(y0, y1);

    // The lagging dimension is that which is not being iterated over, or the
    // one with the shorter of dx and dy
    int *lag = dxGreater ? &y : &x;

    // The error accumulated is always in the dimension corresponding to the
    // shorter of dx and dy, as we "walk" in that direction until we stray
    // too far.  The compensation is thus in the perpendicular direction.
    // We must negate one of these whenever they have the same sign so that
    // one actually compensates for the other.
    float errGain = dxGreater ? dy : dx;
    float errComp = (dxGreater ? dx : dy) * (m > 0 ? -1 : 1);

    // We always increment the iteration variable, so we turn to the lag
    // variable, which increments when the slope is positive and decrements
    // otherwise.
    int step = m > 0 ? 1 : -1;

    // 0 <= m <= 1
    for (; *i < iBnd; ++(*i)) {
        // Fill in this pixel
        (*this)[y][x] = color;
        // Adding dy to our error no matter what
        e += errGain;
        // Check if our error is getting too far from the line of interest such
        // that we need to move in both directions, because the line is closer
        // to the center of the pixel after moving two directions
        if (fabs(2 * e) >= fabs(errComp)) {
            // If so, move to get closer to the line.
            e += errComp;
            *lag += step;
        }
    }
}

/*
 image::generateWireframe

 Takes a shape whose vertices all have a zeroed z coordinate because they are
 already projected onto a screen, and connects the vertices according to the
 instructions contained in the facet vector.

 Arguments: shape3D shape - The shape to create a wireframe out of
            uint32_t color - The color of the wires in the wireframe

 Returns:   Nothing.
*/
void image::generateWireframe
(
    shape3D             shape,
    uint32_t            color
)
{
    vertex v1, v2, v3;

    // Draw a triangle for every facet
    vector<facet>::iterator f = shape.facets->begin();
    for (; f != shape.facets->end(); ++f) {
        // Get all of the vertices to make things quicker and neater
        v1 = shape.vertices->at(f->v1);
        v2 = shape.vertices->at(f->v2);
        v3 = shape.vertices->at(f->v3);

        // Draw each of the three sides of the triangle
        this->drawLine(point2D(v1.x, v1.y), point2D(v2.x, v2.y), color);
        this->drawLine(point2D(v2.x, v2.y), point2D(v3.x, v3.y), color);
        this->drawLine(point2D(v3.x, v3.y), point2D(v1.x, v1.y), color);
    }
}

/*
 image::generateWireframes

 Takes shapes whose vertices all have zeroed z coordinates because they are
 already projected onto a screen, and connects the vertices according to the
 instructions contained in the facet vectors.

 Arguments: vector<shape3D> shape - The shapes to create wireframes out of
            uint32_t color - The color of the wires in the wireframe

 Returns:   Nothing.
*/
void image::generateWireframes
(
    vector<shape3D>     shapes,
    uint32_t            color
)
{
    // Iterate through all the facets in the object, and use the indices to
    // connect points from the pts array (which have been transformed)
    vector<shape3D>::iterator s = shapes.begin();
    for (; s != shapes.end(); ++s) {
        this->generateWireframe(*s, color);
    }
}

/*
 image::outputPPM

 Outputs the argued image according to PPM specifications.

 Arguments: None.

 Returns:   Nothing.
 */
void image::outputPPM
(
)
{
    // Required PPM header
    cout << "P3" << endl;
    // x and y resolutions of the image
    cout << this->xres << " " << this->yres << endl;
    // Maximum intensity of the image
    cout << this->intensity << endl;

    // Print all of the pixel RGB values
    for (int y = this->yres-1; y >= 0; --y) {
        for (int x = 0; x < this->xres; ++x) {
            uint32_t color = (*this)[y][x];
            for (int i = 3; i > 0; --i) {
                uint32_t rgb = color & (255 << (i * 8));
                rgb >>= (i * 8);
                cout << rgb << (i == 1 ? "" : " ");
            }
            cout << endl;
        }
    }
}

/*
 image::rasterTriangle

 This takes one facet and information about the world around it and decides how
 or if to draw to the image.  If the triangle is out of view, it is not drawn.
 If the triangle is in view, then what exact color it should be is computed
 based on either Gouraud or Phong shading algorithm.  Which is used is a
 parameter to the function.

 Arguments: facet f - The facet to display
            shape3D shape - Contains information about the material of f
            camera cam - Contains information about what can and can't be seen
                by the view point
            vector<light> lights - A vector of light sources in the system
            int maxIntensity - The rgb values will be scaled from 0 to
                this number.  This cannot exceed 255.
            shading alg - Defines which algorithm to use when shading

 Returns:   Nothing.
*/
void image::rasterTriangle
(
    facet               f,
    shape3D             shape,
    camera              cam,
    vector<light>       lights,
    int                 maxIntensity,
    shading             alg
)
{
    // Vertices
    vertex v1Orig = shape.vertices->at(f.v1);
    vertex v2Orig = shape.vertices->at(f.v2);
    vertex v3Orig = shape.vertices->at(f.v3);

    // Convert vertices to NDC
    // First get the transformation matrix that can convert world space to
    // camera space
    MatrixXd toCam(4, 4);
    worldToCameraMatrix(cam, &toCam);

    // Get the perspective projection matrix that we will use to get from a
    // point in camera space to our screen
    MatrixXd toNDC(4, 4);
    perspectiveProjectionMatrix(cam, &toNDC);

    // Populate our vertices with Cartesian NDC coordinates
    vertex v1NDC, v2NDC, v3NDC;
    v1Orig.worldToCartNDC(shape.ptTransform, toCam, toNDC, &v1NDC);
    v2Orig.worldToCartNDC(shape.ptTransform, toCam, toNDC, &v2NDC);
    v3Orig.worldToCartNDC(shape.ptTransform, toCam, toNDC, &v3NDC);

    // If the vertex is not facing the camera, we do not want to see it.
    if (!facingCamera(v1NDC, v2NDC, v3NDC))
        return;

    // Normals for computing light
    normal n1 = shape.normals->at(f.n1);
    normal n2 = shape.normals->at(f.n2);
    normal n3 = shape.normals->at(f.n3);

    if (alg == Gouraud) {
        // Compute the color for each vertex
        v1Orig.computeLight(n1, shape.ambient, shape.diffuse, shape.specular,
            shape.shininess, cam, lights);
        v2Orig.computeLight(n2, shape.ambient, shape.diffuse, shape.specular,
            shape.shininess, cam, lights);
        v3Orig.computeLight(n3, shape.ambient, shape.diffuse, shape.specular,
            shape.shininess, cam, lights);

        // Copy colors over
        v1NDC.c = v1Orig.c;
        v2NDC.c = v2Orig.c;
        v3NDC.c = v3Orig.c;
    }

    // Vertices in screen coordinates
    vertex v1, v2, v3;
    v1NDC.NDCToImage(this->xres, this->yres, &v1);
    v2NDC.NDCToImage(this->xres, this->yres, &v2);
    v3NDC.NDCToImage(this->xres, this->yres, &v3);

    // Define our iteration bounds based on the bounds of the facet
    float xMin = min(v1.x, min(v2.x, v3.x));
    float yMin = min(v1.y, min(v2.y, v3.y));
    float xMax = max(v1.x, max(v2.x, v3.x));
    float yMax = max(v1.y, max(v2.y, v3.y));

    // Values used to determine whether point is on interior or exterior of
    // triangle
    float alpha, beta, gamma;
    // Will contain color of each new pixel that we paint
    uint32_t color = 0;

    // Color each point iff. it is inside the triangle described by v1 v2 v3
    for (int x = xMin; x < xMax; ++x) {
        for (int y = yMin; y < yMax; ++y) {
            alpha = v1.barycentricCoeff(v2, v3, point3D(x, y, 0));
            beta =  v2.barycentricCoeff(v1, v3, point3D(x, y, 0));
            gamma = v3.barycentricCoeff(v1, v2, point3D(x, y, 0));

            // If alpha, beta and gamma are all between 0 and 1, the point is
            // inside the triangle
            point3D botCorner(0, 0, 0);
            point3D topCorner(1, 1, 1);
            if (point3D(alpha, beta, gamma).inCube(botCorner, topCorner)) {
                // Find the weighted vertex value.  This will be used to decide
                // whether the point is behind something already in view.
                vertex vNDC =
                    vertex(alpha*v1NDC.x + alpha*v2NDC.x + alpha*v3NDC.x,
                            beta*v1NDC.y +  beta*v2NDC.y +  beta*v3NDC.y,
                           gamma*v1NDC.z + gamma*v2NDC.z + gamma*v3NDC.z);

                // Check if the point is in front of everything else.  Note
                // that the z axis is reversed, so the front is the most
                // negative.
                if (vNDC.z > this->depths[y][x])
                    continue;
                this->depths[y][x] = vNDC.z;

                // PHONG
                if (alg == Phong) {
                    // Find the weighted normal value
                    normal nNDC =
                        normal(alpha*n1.x + alpha*n2.x + alpha*n3.x,
                                beta*n1.y +  beta*n2.y +  beta*n3.y,
                               gamma*n1.z + gamma*n2.z + gamma*n3.z);

                    // Now that we have a sort of average vertex and normal,
                    // find the appropriate color.
                    vNDC.computeLight(n1, shape.ambient, shape.diffuse,
                        shape.specular, shape.shininess, cam, lights);
                    color = vNDC.c.toUInt32(maxIntensity);
                } else if (alg == Gouraud) {
                    // GOURAUD
                    // Update the colors based on where on the facet we are
                    rgb c;
                    c.r = alpha*v1NDC.c.r + beta*v2NDC.c.r + gamma*v3NDC.c.r;
                    c.g = alpha*v1NDC.c.g + beta*v2NDC.c.g + gamma*v3NDC.c.g;
                    c.b = alpha*v1NDC.c.b + beta*v2NDC.c.b + gamma*v3NDC.c.b;
                    color = c.toUInt32(maxIntensity);
                }
                // Fill the pixel with the color computed based on light,
                // texture, distance, etc.
                (*this)[y][x] = color;
            }
        }
    }
}

/*
 image::rasterShape

 Takes a shape and rasters every facet on it.

 Arguments: shape3D shape - The shape to raster
            camera cam - Information about the view of the shape
            vector<light> lights - List of light sources in the system
            shading alg - The shading algorithm to use

 Returns:   Nothing.
*/
void image::rasterShape
(
    shape3D             shape,
    camera              cam,
    vector<light>       lights,
    shading             alg
)
{
    // Want to raster every face on the shape
    std::vector<facet>::iterator f = shape.facets->begin();
    for (; f != shape.facets->end(); ++f) {
        this->rasterTriangle(*f, shape, cam, lights, this->intensity, alg);
    }
}

/*
 image::rasterShapes

 Takes a list of shapes and rasters each of them.

 Arguments: vector<shape3D> shapes - The list of shapes to raster
            camera cam - Information about the view of the shape
            vector<light> lights - List of light sources in the system
            shading alg - The shading algorithm to use

 Returns:   Nothing.
*/
void image::rasterShapes
(
    vector<shape3D>     shapes,
    camera              cam,
    vector<light>       lights,
    shading             alg
)
{
    vector<shape3D>::iterator s = shapes.begin();
    for (; s != shapes.end(); ++s) {
        this->rasterShape(*s, cam, lights, alg);
    }
}
