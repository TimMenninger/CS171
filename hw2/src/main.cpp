/******************************************************************************

 main.cpp

 Main loop for HW2.  This reads from the command line a file, x and y
 resolutions and an algorithm code.  Then, it draws the image and outputs in
 PPM format.

 Author: Tim Menninger

******************************************************************************/
#include "parseScene.h"
#include "image.h"
#include "geom.h"
#include "light.h"

#define BKG_COLOR       0x000000ff // black, 0% transparent
#define MAX_INTENSITY   255        // Max intensity of any one pixel color

using namespace std;

int main(int argc, char **argv) {
    if (argc != 5) {
        cout << "usage: ./shaded [file] [yres] [xres] [mode]" << endl;
        return 1;
    }
    int xres = atoi(argv[2]), yres = atoi(argv[3]), mode = atoi(argv[4]);

    shading alg;
    // Determine which algorithm to use.
    switch (mode) {
        case 0:
            alg = Gouraud;
            break;
        case 1:
            alg = Phong;
            break;
        default:
            cout << mode << " does not match an algorithm" << endl;
            return 1;
    }

    // Parse the file for the camera, objects and transformations
    camera cam;
    vector<light> lights;
    vector<string> order;
    map<string, shape3D*> originals;
    vector<shape3D> copies;
    int status = parseScene(argv[1], &cam, &lights,
        &order, &originals, &copies);

    // Take the NDC coordinates and get their projections onto the screen
    image im(xres, yres, MAX_INTENSITY, BKG_COLOR);
    im.rasterShapes(copies, cam, lights, alg);
    im.outputPPM();

    return 0;
}
