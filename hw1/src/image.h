#ifndef IMAGE
#define IMAGE

#include <stdint.h>
#include <cmath>

#include "projection.h"
#include "geom.h"

// Structs

/*
 image

 Represents an image on a screen.
*/
typedef struct _image {
    int         xres;
    int         yres;
    int         intensity;
    uint32_t    **colors;

    // Create a 2D vector of pixels given xres and yres
    _image(int xres, int yres, int i, uint32_t rgba)
        : xres (xres), yres (yres), intensity (i)
    {
        colors = (uint32_t **)malloc(yres * sizeof(uint32_t *));
        for (int r = 0; r < yres; ++r) {
            colors[r] = (uint32_t *)malloc(xres * sizeof(uint32_t));
            for (int c = 0; c < xres; ++c) {
                colors[r][c] = rgba;
            }
        }
    }
    ~_image() {
        for (int r = 0; r < yres; ++r)
            free (colors[r]);
    }

    // Allow us to access colors array with square brackets
    uint32_t* &operator[] (unsigned int i) { return colors[i]; }

    void outputPPM();
    void drawLine (point2D, point2D, uint32_t);
    void generateWireframes(std::vector<shape3D>, uint32_t);
    void generateWireframe(shape3D, uint32_t);

} image;

void NDCToScreen (int, int, camera*, std::vector<shape3D>, std::vector<shape3D>*);

#endif // ifndef IMAGE
