/******************************************************************************

 image.cpp

 Contains methods that convert objects whose points are in NDC to be viewable
 on a 2D screen.

 Author: Tim Menninger

******************************************************************************/
#include "image.h"

using namespace std;

/*
 NDCVertexToImage

 Projects a vertex in NDC onto the 2D screen from which we are viewing.

 Arguments: int xres - X resolution of the image in pixels
            int yres - Y resolution of the image in pixels
            camera *cam - The camera view information
            vertex v - The vertex to project onto NDC
            shape3D *out - The projected vertex is stored in the vertices
                attribute of this shape3D

 Returns:   Nothing.
*/
void NDCVertexToImage
(
    int                 xres,
    int                 yres,
    camera              *cam,
    vertex              v,
    shape3D             *out
)
{
    // Only want to add this point if it is within (-1, -1, -1) and (1, 1, 1)
    float x = v.x, y = v.y, z = v.z;
    if (x > 1 || y > 1 /*|| z > 1*/ || x < -1 || y < -1 /*|| z < -1*/)
        return;

    // Point is in view, scale NDC (-1, 1) to (0, xres) and (0, yres) and add
    // it to the image
    int winX = (1 + x) * (xres / 2);
    int winY = ((1 + y) * (yres / 2));
    out->vertices->push_back(vertex(winX, winY, 0));
}

/*
 NDCShapeToScreen

 Projects a shape whose vertices are in NDC onto a 2D screen.

 Arguments: int xres - X resolution of the image in pixels
            int yres - Y resolution of the image in pixels
            camera *cam - The camera view information
            shape3D origShape - The original shape, in NDC
            shape3D *out - The projected vertex is stored in the vertices
                attribute of this shape3D

 Returns:   Nothing.
*/
void NDCShapeToScreen
(
    int                 xres,
    int                 yres,
    camera              *cam,
    shape3D             origShape,
    shape3D             *outShape
)
{
    // Project every vertex in the origShape onto the screen image
    vector<vertex>::iterator v = origShape.vertices->begin();
    for (; v != origShape.vertices->end(); ++v) {
        NDCVertexToImage(xres, yres, cam, *v, outShape);
    }

    // Copy all of the facets
    vector<facet>::iterator f = origShape.facets->begin();
    for (; f != origShape.facets->end(); ++f) {
        outShape->facets->push_back(*f);
    }
}

/*
 NDCToScreen

 Projects a vector of shapes whose vertices are all in NDC onto a 2D screen,
 saving the projected objects in a supplied vector.

 Arguments: int xres - X resolution of the image in pixels
            int yres - Y resolution of the image in pixels
            camera *cam - The camera view information
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
        NDCShapeToScreen(xres, yres, cam, *shape, &scrShape);
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
            image *im - The image that the shape will appear on
            uint32_t color - The color of the wires in the wireframe

 Returns:   Nothing.
*/
void image::generateWireframe
(
    shape3D             shape,
    uint32_t            color
)
{
    // Draw a triangle for every facet
    vector<facet>::iterator f = shape.facets->begin();
    for (; f != shape.facets->end(); ++f) {
        // Get all of the vertices to make things quicker and neater
        vertex v1 = shape.vertices->at(f->v1);
        vertex v2 = shape.vertices->at(f->v2);
        vertex v3 = shape.vertices->at(f->v3);

        // Draw each of the three sides of the triangle
        this->drawLine(point2D(v1.x, v1.y), point2D(v2.x, v2.y), color);
        this->drawLine(point2D(v2.x, v2.y), point2D(v3.x, v3.y), color);
        this->drawLine(point2D(v3.x, v3.y), point2D(v1.x, v1.y), color);
    }
}

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
    for (int y = 0; y < this->yres; ++y) {
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
