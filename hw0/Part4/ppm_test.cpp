#include "ppm_circle.h"

using namespace std;


/*
 drawCenteredCircle

 Creates an image and draws a circle in the center of it.

 Arguments: int xres - Pixels in x direction of image
            int yres - Pixels in y direction of image
            int *color - RGB array of circle color
            int *bgColor - RGB array of background color
            int ***image - 2D array of RGB arrays describing image

 Returns:   Nothing.
 */
void drawCenteredCircle
(
    int xres,
    int yres,
    int *color,
    int *bgColor,
    int ***image
)
{
    assert(color);
    assert(bgColor);
    assert(image);

    // Create an image of desired size and color
    newImage(xres, yres, bgColor, image);

    // Draw a circle on the new image
    int cx = xres / 2; // Center x of the circle
    int cy = yres / 2; // Center y of the circle
    int r = (xres < yres ? xres / 4 : yres / 4); // Radius of circle
    drawCircle(xres, yres, cx, cy, r, color, image);

    return;
}

/*
 writePPM

 Outputs the argued image according to PPM specifications.

 Arguments: int xres - The number of pixels in the x direction
            int yres - Number of pixels in y direction
            int intensity - The maximum intensity of a pixel
            int ***image - 2D array of RGB arrays describing image

 Returns:   Nothing.
 */
void writePPM
(
    int xres,
    int yres,
    int intensity,
    int ***image
)
{
    assert(image);
    assert(*image);
    assert(**image);

    // Required PPM header
    cout << "P3" << endl;
    // x and y resolutions of the image
    cout << xres << " " << yres << endl;
    // Maximum intensity of the image
    cout << intensity << endl;

    // Print all of the pixel RGB values
    for (int y = 0; y < yres; ++y) {
        for (int x = 0; x < xres; ++x) {
            for (int rgb = 0; rgb < 3; ++rgb) {
                cout << image[y][x][rgb] << (rgb == 2 ? "" : " ");
            }
            cout << endl;
        }
    }
}

/*
 main

 Draws a centered circle that is a different color than the background
 */
int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "usage: ./ppm_test <xres> <yres>";
        return 1;
    }

    // Parse arguments
    int xres = atoi(argv[1]), yres = atoi(argv[2]);

    // Draw the circle
    int ***image = (int ***)malloc(yres * sizeof(int **));
    if (!image) {
        exit(1);
    }
    int color[3] = CIRCLE_COLOR;
    int bgColor[3] = BG_COLOR;
    drawCenteredCircle(xres, yres, color, bgColor, image);

    // Print the image
    writePPM(xres, yres, MAX_INTENSITY, image);

    return 0;
}
