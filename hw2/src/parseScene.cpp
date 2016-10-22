/******************************************************************************

 parseScene.cpp

 Reads in a scene and fills the appropriate objects for other files to
 manipulate and create a scene out of.

 Author: Tim Menninger

******************************************************************************/

#include "parseScene.h"

using namespace std;
using namespace Eigen;

/*
 generateMatrix

 For each line, we will create a matrix of 4 dimensions.  This only works for
 3D systems.  This parses the file until either an empty line or the end of
 the file.
    t   translation vector (tx, ty, tz) creates matrix
            [ 1  0  0 tx ]
            [ 0  1  0 ty ]
            [ 0  0  1 tz ]
            [ 0  0  0  1 ]
    r   rotation vector and rotation degrees (rx, ry, rz, theta)
        in radians, where rx, ry, rz define the axis around which the
        object will be rotated counterclockwise by theta radians.
    s   scalar matrix (sx, sy, sz) creates matrix
            [ sx   0   0   0 ]
            [  0  sy   0   0 ]
            [  0   0  sz   0 ]
            [  0   0   0   1 ]

 Arguments: ifstream *openFile - The file being read
            vector<MatrixXd> *ptMatrices - The matrices created from the
                instructions in the file.
            vector<MatrixXd> *normMatrices - All matrices from the file
                that aren't translations, to be used on normals

 Returns:   (int) - Zero if successful, nonzero otherwise.
*/
int generateMatrix
(
    ifstream            *openFile,
    vector<MatrixXd>    *ptMatrices,
    vector<MatrixXd>    *normMatrices
)
{
    assert(openFile);
    assert(ptMatrices);
    assert(normMatrices);

    ptMatrices->clear();
    normMatrices->clear();

    // The string buffer that will be read
    string line;
    // Read each line and create an element
    while (getline(*openFile, line)) {
        // Will be the matrix that is added next
        MatrixXd m(4, 4);

        // Generate the matrix from the instruction on this line
        if (generateTransform(line, &m) == 0)
            return 0;

        ptMatrices->push_back(m);
        if (line[0] != 't')
            normMatrices->push_back(m);
    }

    return 0;
}

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
    vector<string> vals;
    string line;
    // Trash the first line, which is just a token
    getline(*inFile, line);
    while (getline(*inFile, line)) {

        // Split the space-separated line into a vector of strings
        vals = getSpaceDelimitedWords(line);

        // An empty line separates the files from the transforms
        if (vals.size() == 0) {
            return;
        }

        // The first value is the name, the second is the file
        shape3D *orig = new shape3D();
        orig->vertices = new vector<vertex>;
        orig->facets = new vector<facet>;
        orig->normals = new vector<normal>;
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
 getLightSources

 Reads the file for light sources and fills the argued vector with all the
 lights found.  Lights should be of the form:
    light <xyz point> , <rgb color> , <attenuation>
 where xyz point contains three floats, and rgb color contains three floats
 in the range [0, 1], inclusive.

 Arguments: ifstream *inFile - The file to parse for light sources, starting
                at the next readable line.
            vector<light> *lights - Filled with light sources parsed from the
                file.

 Returns:   Nothing.
*/
void getLightSources
(
    ifstream                *inFile,
    vector<light>           *lights
)
{
    assert (inFile);
    assert (lights);

    string line;
    vector<string> vals;
    while (getline(*inFile, line)) {
        vals = getSpaceDelimitedWords(line);

        // If we are at a blank line, we are done with the section
        if (vals.size() == 0) {
            return;
        }

        // Load the light source information into a light object
        point3D source = point3D(stof(vals[1]), stof(vals[2]), stof(vals[3]));
        rgb color = rgb(stof(vals[5]), stof(vals[6]), stof(vals[7]));
        light light(source, color, stof(vals[9]));

        lights->push_back(light);
    }
}

/*
 getTransformedShapes

 Takes a file and parses it for transformations to be done on shapes.  It
 creates the appropriate matrices and applies the transforms onto copies of
 the original objects.

 Arguments: ifstream *inFile - The file to parse
            map<string, shape3D*> - Maps object names to their objects so we
                know which object to modify during parsing
            vector<shape3D> *out - Filled with transformed shapes

 Returns:   Nothing.
*/
void getTransformedShapes
(
    ifstream                        *inFile,
    map<string, shape3D*>           *originals,
    vector<shape3D>                 *out
)
{
    // The next sets of lines each describe a transformation on an object,
    // each being prefaced by the object name
    string line;
    vector<string> vals;
    while (getline(*inFile, line)) {
        // Create the empty shape and give it an approprate name
        shape3D newShape;
        string objName = line.substr(0, line.length());

        // Give the name and copy number to the new shape
        newShape.name = objName + "_copy";
        newShape.vertices = new vector<vertex>;
        newShape.facets = new vector<facet>;
        newShape.normals = new vector<normal>;

        newShape.copy((*originals)[objName]);

        // The first lines are ambient, diffuse, specular, shininess
        getline(*inFile, line);
        vals = getSpaceDelimitedWords(line);
        newShape.ambient = rgb(stof(vals[1]), stof(vals[2]), stof(vals[3]));
        getline(*inFile, line);
        vals = getSpaceDelimitedWords(line);
        newShape.diffuse = rgb(stof(vals[1]), stof(vals[2]), stof(vals[3]));
        getline(*inFile, line);
        vals = getSpaceDelimitedWords(line);
        newShape.specular = rgb(stof(vals[1]), stof(vals[2]), stof(vals[3]));
        getline(*inFile, line);
        vals = getSpaceDelimitedWords(line);
        newShape.shininess = stof(vals[1]);

        // Create a transformation matrix from the instructions
        MatrixXd ptTransMatrix = MatrixXd::Identity(4, 4);
        MatrixXd normTransMatrix = MatrixXd::Identity(4, 4);
        vector<MatrixXd> ptMatrices;
        vector<MatrixXd> normMatrices;
        generateMatrix(inFile, &ptMatrices, &normMatrices);
        transformMatrix(&newShape.ptTransform, &ptMatrices);
        transformMatrix(&newShape.normTransform, &normMatrices);
        normTransMatrix = normTransMatrix.inverse().transpose();

        // Add the shape to the output
        out->push_back(newShape);
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
    light <xyz point> , <rgb color> , <attenuation
    ... // Some number of light sources
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
    vector<light>                   *lights,
    vector<string>                  *order,
    map<string, shape3D*>           *originals,
    vector<shape3D>                 *out
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

    // Get light source info
    getLightSources(&inFile, lights);

    // The next line(s) are objects and names for those objects
    getOriginalShapes(&inFile, order, originals);

    // The next line(s) are shapes to copy and transformations to do
    getTransformedShapes(&inFile, originals, out);

    return 0;
}
