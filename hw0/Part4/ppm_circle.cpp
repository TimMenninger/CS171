#include "ppm_circle.h"

using namespace std;

/*
 newImage

 Creates a new, blank image with the argued dimensions and color.  It returns
 with the image in the argued triple-pointer.

 Arguments: int xres - Pixels in x direction
            int yres - Pixels in y direction
            int *color - RGB array for the default color of the image
            int ***image - 2D array of RGB arrays describing image

 Returns:   Nothing.
 */
void newImage
(
    int xres,
    int yres,
    int *color,
    int ***image
)
{
    assert(color);
    assert(image);

    // For yres rows in the image, put xres RGB values
    for (int r = 0; r < yres; ++r) {
        image[r] = (int **)malloc(xres * sizeof(int *));
        // Ensure successful malloc
        if (!image[r])
            exit(1);

        // Fill the row
        for (int c = 0; c < xres; ++c) {
            image[r][c] = (int *)malloc(3 * sizeof(int));
            // Ensure successful malloc
            if (!image[r][c])
                exit(1);
            // Fill the RGB array with the argued color
            for (int rgb = 0; rgb < 3; ++rgb) {
                image[r][c][rgb] = color[rgb];
            }
        }
    }
}

/*
 drawCircle

 Draws a cicle on the argued image with the argued radius and center.

 Arguments: int xres - Number of pixels in the x direction
            int yres - Number of pixels in the y direction
            int cx - Center x coordinate of the circle
            int cy - Center y coordinate of the circle
            int r - Radius of the circle
            int *color - RGB array of color of the circle
            int ***image - 2D array of RGB arrays describing entire image

 Returns:   Nothing.
 */
void drawCircle
(
    int xres,
    int yres,
    int cx,
    int cy,
    int r,
    int *color,
    int ***image
)
{
    assert(color);
    assert(image);
    assert(*image);
    assert(**image);

    // Iterate over all values of (xres, yres), and if it would be inside a
    // circle of radius r at center (cx, cy), color that pixel of the image
    // with the argued color
    for (int y = 0; y < yres; ++y) {
        for (int x = 0; x < xres; ++x) {
            // Circle equation is (x - cx)^2 + (y - cy)^2 <= r^2.  Anything
            // that satisfies this should change color
            int lhs = (x - cx) * (x - cx) + (y - cy) * (y - cy);
            int rhs = r * r;
            if (lhs <= rhs) {
                // If here, the pixel is inside the circle--change color
                for (int rgb = 0; rgb < 3; ++rgb) {
                    *(image[y][x] + rgb) = color[rgb];
                }
            }
        }
    }
}
