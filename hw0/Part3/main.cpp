#include "transformObjs.h"

using namespace std;

/*
 main

 Reads a file with objects and transformation instructions.  It copies the
 objects for each specified transformation.
*/
int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "usage: ./transformObjs <file>" << endl;
        return 1;
    }

    vector<string> order;
    map<string, shape3D*> originals;
    map<string, vector<shape3D> > shapes;
    int status = parseShapeTransforms(argv[1], &order, &originals, &shapes);

    printShapes(&order, &originals, &shapes);
    cleanupShapes(&originals, &shapes);

    return status;
}
