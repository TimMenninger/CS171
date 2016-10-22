/******************************************************************************

 image.h

 Contains image struct and declares public functions in image.cpp.

 Author: Tim Menninger

******************************************************************************/
#ifndef IMAGE
#define IMAGE

#include <cmath>

#include <Eigen/Dense>

#include "geom.h"
#include "transform.h"
#include "light.h"
#include "projection.h"

// Enums
/*
 shading

 Used to determine which shading algorithm to use when rastering the image.
*/
typedef enum _shading {
    Gouraud,
    Phong,
} shading;

// Structs

/*
 image

 Represents an image on a screen.  It contains screen resolution, the max
 intensity of a pixel color, the pixel colors themselves and the depth of the
 frontmost object at each location.
*/
typedef struct _image {
    int         xres;
    int         yres;
    int         intensity;
    uint32_t    **colors;
    uint32_t    **depths;

    // Create a 2D vector of pixels given xres and yres
    _image(int xres, int yres, int i, uint32_t rgba)
        : xres (xres), yres (yres), intensity (i)
    {
        colors = (uint32_t **)malloc(yres * sizeof(uint32_t *));
        depths = (uint32_t **)malloc(yres * sizeof(uint32_t *));
        for (int r = 0; r < yres; ++r) {
            colors[r] = (uint32_t *)malloc(xres * sizeof(uint32_t));
            depths[r] = (uint32_t *)malloc(xres * sizeof(uint32_t));
            for (int c = 0; c < xres; ++c) {
                colors[r][c] = rgba;
                depths[r][c] = -1;
            }
        }
    }
    ~_image() {
        for (int r = 0; r < yres; ++r) {
            free (colors[r]);
            free (depths[r]);
        }
    }

    // Allow us to access colors array with square brackets
    uint32_t* &operator[] (unsigned int i) { return colors[i]; }

    void outputPPM ();

    void drawLine (point2D, point2D, uint32_t);
    void generateWireframes (std::vector<shape3D>*, uint32_t);
    void generateWireframe (shape3D*, uint32_t);

    void rasterTriangle (facet, shape3D*, camera, std::vector<light>, int, shading);
    void rasterShapes (std::vector<shape3D>*, camera, std::vector<light>, shading);
    void rasterShape (shape3D*, camera, std::vector<light>, shading);

} image;

// Function handles
void NDCToScreen (int, int, camera*, std::vector<shape3D>*, std::vector<shape3D>*);

#endif // ifndef IMAGE
