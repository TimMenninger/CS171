/******************************************************************************

 parseScene.cpp

 Reads in objects, and makes a copy for each desired transformation

 Author: Tim Menninger

******************************************************************************/

#include "parseScene.h"

using namespace std;
using namespace Eigen;

#define PI 3.14159262


/*
 getOriginalShapes

 Takes an open file, reads a line containing an object name and filepath,
 and fills a shape with its contents.  It loads the shape into a map indexed
 by the name.  It assumes that inFile points to a location in a file that
 looks like:
    objects:
    <name1> <OBJ file1>
    ...
    <nameN> <OBJ fileN>
    <newline>

 Arguments: ifstream *inFile - The file containing object names and paths
            vector<string> *order - The order of objects read in
            map<string, shape3D*> *originals - Maps object names to the
                object

 Returns:   Nothing.
 */
void getOriginalShapes
(
    ifstream                *inFile,
    vector<string>          *order,
    map<string, shape3D*>   *originals
)
{
    assert(inFile);
    assert(order);
    assert(originals);

    order->clear();
    string line;
    // Trash the first line, which is just a token
    getline(*inFile, line);
    while (getline(*inFile, line)) {

        // Split the space-separated line into a vector of strings
        vector<string> vals = getSpaceDelimitedWords(line);

        // An empty line separates the files from the transforms
        if (vals.size() == 0) {
            return;
        }

        // The first value is the name, the second is the file
        shape3D *orig = new shape3D();
        orig->vertices = new vector<vertex>;
        orig->facets = new vector<facet>;
        // Parse the obj file we obtained from this description
        parseObjFile(OBJ_DIR + vals[1], orig);
        orig->name = vals[0];
        // Save the object
        order->push_back(vals[0]);
        (*originals)[vals[0]] = orig;
    }
}

/*
 getCameraInfo

 Parses the in file and fills a camera struct with the information in it.  It
 is assumed that the argued file stream contains information pointing to
 somewhere in a file that looks like this:
    camera:
    position <float x> <float y> <float z>
    orientation <ffloat x> <float y> <float z> <float angle>
    near <float>
    far <float>
    left <float>
    right <float>
    top <float>
    bottom <float>
    <newline>
    <rest of file>

 Arguments: ifstream *inFile - The file to parse for camera info
            camera *cam - A pointer to where to store the camera information
                read fromt he file.

 Returns:   Nothing.
*/
void getCameraInfo
(
    ifstream                *inFile,
    camera                  *cam
)
{
    assert(inFile);
    assert(cam);

    string line;
    // Splitting each space-separated line into a vector of strings
    vector<string> vals;
    // Trash the first line, which contains the "camera" token
    getline(*inFile, line);

    // First value is the camera position
    getline(*inFile, line);

    vals = getSpaceDelimitedWords(line);
    float x = strtof(vals[1].c_str(), NULL);
    float y = strtof(vals[2].c_str(), NULL);
    float z = strtof(vals[3].c_str(), NULL);
    cam->pos = point3D(x, y, z);

    // Second value is camera orientation
    getline(*inFile, line);

    vals = getSpaceDelimitedWords(line);
    x = strtof(vals[1].c_str(), NULL);
    y = strtof(vals[2].c_str(), NULL);
    z = strtof(vals[3].c_str(), NULL);
    float t = strtof(vals[4].c_str(), NULL);
    cam->orient = orientation(x, y, z, t);

    // Last six values are all floats.  Will use offset to fill rest of struct
    int offset = sizeof(point3D) + sizeof(orientation);
    char *addr = (char *) cam;
    while (getline(*inFile, line)) {
        vals = getSpaceDelimitedWords(line);

        // If we are at a blank line, we are done with the camera
        if (vals.size() == 0) {
            return;
        }

        // Load the next value (as a float)
        *(float *) (addr + offset) = strtof(vals[1].c_str(), NULL);
        // Increment our offset for the next iteration
        offset += sizeof(float);
    }
}

/*
 transformShape

 Takes an original shape and a transformation matrix and applies the
 transformation.  It fills an argued shape pointer with the resultant shape.

 Arguments: shape3D *origShape - The original shape object
            MatrixXd *matrix - The transformation matrix
            shape3D *transformed - Pointer to the shape to fill with the
                result

 Returns:   Nothing.
 */
void transformShape
(
    shape3D     *origShape,
    MatrixXd    *matrix,
    shape3D     *transformed
)
{
    assert(origShape);
    assert(matrix);
    assert(transformed);

    // Ensure we are starting with an empty shape
    transformed->clear();

    // Apply the transformation to every point in the copy
    vector<vertex>::iterator it = origShape->vertices->begin();
    for (; it != origShape->vertices->end(); ++it) {
        // Create a vector from the vertex
        MatrixXd vOld(4, 1);
        vOld << it->x, it->y, it->z, 1;

        // Transform the vertex
        MatrixXd vNew = *matrix * vOld;
        vertex v(vNew(0), vNew(1), vNew(2));
        transformed->vertices->push_back(v);
    }

    // The set of faces has the same vertices (only the vertices move)
    vector<facet>::iterator face = origShape->facets->begin();
    for (; face != origShape->facets->end(); ++face) {
        transformed->facets->push_back(*face);
    }
}

/*
 parseScene

 Iterates through lines in the file with the argued name.  It expects the first
 lines to be shape names and filepaths.  It reads those and saves them.  Then,
 an empty line, then a set of transformations.  Each transformation is
 separated by an empty line and has the form of <shape_name>\n<transformations>
 with each transformation on a new line.  It fills a vector with the order
 objects were read, a map of names to original objects, and a map of names
 to a vector of copies of that original object.  The file argued should have
 the form:
    camera:
    position <float x> <float y> <float z>
    orientation <ffloat x> <float y> <float z> <float angle>
    near <float>
    far <float>
    left <float>
    right <float>
    top <float>
    bottom <float>
    <newline>
    objects:
    <name1> <OBJ file1>
    ... // Some number of objects
    <nameN> <OBJ fileN>
    <newline>
    <name in [name1, nameN]>
    <transformation 1>
    ... // Some number of transformations
    <transformation M>
    <newline>
    ... // Repeat for some number of objects (can contain repeats)
    <name in [name1, nameN]>
    <transformation 1>
    ... // Some number of transformations
    <transformation P>

 Arguments: char *filename - The name of the file to parse
            camera *cam - An empty camera instance to fill with what is read
                in the file.
            vector<string> - A vector of object names in the order they were
                read.
            map<string, shape3D*> *originals - Maps object names to the
                object definition.
            map<string, vector<shape3D> > *out - Maps object names to a
                vector of its transformed copies.

 Returns:   (int) - 0 if successful, nonzero otherwise
*/
int parseScene
(
    char                            *filename,
    camera                          *cam,
    vector<string>                  *order,
    map<string, shape3D*>           *originals,
    map<string, vector<shape3D> >   *out
)
{
    assert(filename);
    assert(order);
    assert(originals);
    assert(out);

    // Read the file
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "unable to open " << filename << endl;
        return 1;
    }
    // The first lines describe the camera
    getCameraInfo(&inFile, cam);

    // The next line(s) are objects and names for those objects
    getOriginalShapes(&inFile, order, originals);

    // The next sets of lines each describe a transformation on an object,
    // each being prefaced by the object name
    string line;
    while (getline(inFile, line)) {
        // Create a transformation matrix from the instructions
        MatrixXd transMatrix = MatrixXd::Identity(4, 4);
        vector<MatrixXd> matrices;
        generateMatrix(&inFile, &matrices);
        transformMatrix(&transMatrix, &matrices);

        // Create the empty shape and give it an approprate name
        shape3D transformed;
        string objName = line.substr(0, line.length());
        ostringstream copyNumber;
        copyNumber << (*out)[objName].size() + 1;
        transformed.name = objName + "_copy" + copyNumber.str();
        transformed.vertices = new vector<vertex>;
        transformed.facets = new vector<facet>;
        // Transform the shape
        shape3D *origShape = (*originals)[objName];
        transformShape(origShape, &transMatrix, &transformed);
        // Give the name and copy number to the transformed shape

        // Add the shape to the output
        (*out)[objName].push_back(transformed);
    }
    return 0;
}
