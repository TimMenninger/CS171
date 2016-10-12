#include "parseScene.h"
#include "image.h"
#include "geom.h"

#define BKG_COLOR       0x000000ff // black, 0% transparent
#define WIRE_COLOR      0xffffffff // white, 0% transparent
#define INTENSITY       255 // Max intensity of any one pixel color

using namespace std;

int main(int argc, char **argv) {
    if (argc != 4) {
        cout << "usage: ./transformObjs [file] [yres] [xres]" << endl;
        return 1;
    }
    int xres = atoi(argv[2]), yres = atoi(argv[3]);

    // Parse the file for the camera, objects and transformations
    camera cam;
    vector<string> order;
    map<string, shape3D*> originals;
    map<string, vector<shape3D> > copies;
    int status = parseScene(argv[1], &cam, &order, &originals, &copies);

    // Transform each shape to Cartesian NPC for displaying.  To do this,
    // we first transform the objects to camera space, then we can project
    // them to NPC.
    vector<shape3D> NDCShapes;
    worldToCartNDC(cam, copies, &NDCShapes);

    // Take the NDC coordinates and get their projections onto the screen
    vector<shape3D> screenShapes;
    NDCToScreen(xres, yres, &cam, NDCShapes, &screenShapes);
    image im(xres, yres, INTENSITY, BKG_COLOR);
    im.generateWireframes(screenShapes, WIRE_COLOR);
    im.outputPPM();

    return 0;
}
