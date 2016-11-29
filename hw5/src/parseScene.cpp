/******************************************************************************

 parseScene.cpp

 Reads in a scene and fills the appropriate objects for other files to
 manipulate and create a scene out of.

 Author: Tim Menninger

******************************************************************************/

#include "parseScene.h"

using namespace std;

/*
 getTransforms

 For each line, we will create a Transform for the argued object.

 Arguments: ifstream *openFile - The file being read
            Object *obj - The object to put transformations into

 Returns:   (int) - Zero if successful, nonzero otherwise.
*/
int getTransforms
(
    ifstream            *openFile,
    Object              *obj
)
{
    assert(openFile);
    assert(obj);

    // The string buffer that will be read
    string line;
    // Read each line and create an element
    while (getline(*openFile, line)) {
        // The next transform to add to the object
        Transforms t;

        // Split the space-separated line into a vector of strings
        vector<string> vals = getSpaceDelimitedWords(line);

        // Done when vals doesn't return with anything, as it is moving on
        // to the next copy
        if (vals.size() == 0)
            return 0;

        // Extract the values from the string
        float x = stof(vals[1]), y = stof(vals[2]), z = stof(vals[3]), mag;
        // Will be what gets filled with the transform
        float *transformation;

        switch (line[0]) {
            case 'r':
                // In rotation, we also have a degrees to rotate
                transformation = t.rotation;
                mag = sqrt(x*x + y*y + z*z);
                x /= mag;
                y /= mag;
                z /= mag;
                t.rotation_angle = stof(vals[4]) * 180 / PI;
                break;
            case 't':
                // Fill the translation
                transformation = t.translation;
                break;
            case 's':
                // Fill in scaling vector
                transformation = t.scaling;
                break;
            default:
                cout << "unable to parse file" << endl;
                return -1;
        }
        // Fill in the transformation with the row in the file.
        transformation[0] = x;
        transformation[1] = y;
        transformation[2] = z;

        // Add this new transform to the object
        obj->transform_sets.push_back(t);

    }

    return 0;
}

/*
 getOriginalShapes

 Takes an open file, reads a line containing an object name and filepath,
 and fills a shape with its contents.  It loads the shape into a map indexed
 by the name.

 Arguments: ifstream *inFile - The file containing object names and paths
            map<string, Object*> *originals - Maps object names to the
                object

 Returns:   Nothing.
 */
void getOriginalShapes
(
    ifstream                *inFile,
    map<string, Object*>    *originals
)
{
    assert(inFile);
    assert(originals);

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
        Object *orig = new Object();

        // Initialize the pointers in this object
        orig->mesh.vertices = new vector<Vertex*>;
        orig->mesh.faces    = new vector<Face*>;
        orig->hefs          = new vector<HEF*>;
        orig->hevs          = new vector<HEV*>;

        // Parse the obj file we obtained from this description
        parseObjFile(OBJ_DIR + vals[1], orig);
        orig->name = vals[0];

        // Save the object
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
            Camera *c - The camera struct with information about the camera
                view

 Returns:   Nothing.
*/
void getCameraInfo
(
    ifstream                *inFile,
    Camera                  *c
)
{
    assert(inFile);

    string line;
    // Splitting each space-separated line into a vector of strings
    vector<string> vals;
    // Trash the first line, which contains the "camera" token
    getline(*inFile, line);

    // First value is the camera position
    getline(*inFile, line);

    vals = getSpaceDelimitedWords(line);
    c->cam_position[0] = strtof(vals[1].c_str(), NULL);
    c->cam_position[1] = strtof(vals[2].c_str(), NULL);
    c->cam_position[2] = strtof(vals[3].c_str(), NULL);

    // Second value is camera orientation
    getline(*inFile, line);

    vals = getSpaceDelimitedWords(line);
    c->cam_orientation_axis[0] = strtof(vals[1].c_str(), NULL);
    c->cam_orientation_axis[1] = strtof(vals[2].c_str(), NULL);
    c->cam_orientation_axis[2] = strtof(vals[3].c_str(), NULL);
    c->cam_orientation_angle   = strtof(vals[4].c_str(), NULL) * 180 / PI;

    // Last six values are all floats.  Will use offset to fill rest of struct
    float *view[6] = { &c->near_param, &c->far_param,
                       &c->left_param, &c->right_param,
                       &c->top_param, &c->bottom_param };
    int offset = 0;
    while (getline(*inFile, line)) {
        vals = getSpaceDelimitedWords(line);

        // If we are at a blank line, we are done with the camera
        if (vals.size() == 0) {
            return;
        }

        // Load the next value (as a float)
        *view[offset++] = strtof(vals[1].c_str(), NULL);
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
            vector<Point_Light> *lights - Filled with light sources parsed from
                the file.

 Returns:   Nothing.
*/
void getLightSources
(
    ifstream                *inFile,
    vector<Point_Light>     *lights
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
        Point_Light light;
        light.position[0]   = stof(vals[1]);
        light.position[1]   = stof(vals[2]);
        light.position[2]   = stof(vals[3]);
        light.position[3]   = 1;

        light.color[0]      = stof(vals[5]);
        light.color[1]      = stof(vals[6]);
        light.color[2]      = stof(vals[7]);

        light.attenuation_k = stof(vals[9]);

        lights->push_back(light);
    }
}

/*
 getTransformedShapes

 Takes a file and parses it for transformations to be done on shapes.  It
 creates the appropriate matrices and applies the transforms onto copies of
 the original objects.

 Arguments: ifstream *inFile - The file to parse
            map<string, Object*> - Maps object names to their objects so we
                know which object to modify during parsing
            vector<Object> *out - Filled with transformed shapes

 Returns:   Nothing.
*/
void getTransformedShapes
(
    ifstream                        *inFile,
    map<string, Object*>            *originals,
    vector<Object>                  *out
)
{
    // The next sets of lines each describe a transformation on an object,
    // each being prefaced by the object name
    string line;
    vector<string> vals;
    while (getline(*inFile, line)) {
        // Create the empty shape and give it an approprate name
        string objName = line.substr(0, line.length());
        Object newObj = *(*originals)[objName];
        // Give the name and copy number to the new shape
        newObj.name = objName + "_copy";

        // The first lines are ambient, diffuse, specular, shininess
        getline(*inFile, line);
        vals = getSpaceDelimitedWords(line);
        newObj.ambient_reflect[0] = stof(vals[1]);
        newObj.ambient_reflect[1] = stof(vals[2]);
        newObj.ambient_reflect[2] = stof(vals[3]);

        getline(*inFile, line);
        vals = getSpaceDelimitedWords(line);
        newObj.diffuse_reflect[0] = stof(vals[1]);
        newObj.diffuse_reflect[1] = stof(vals[2]);
        newObj.diffuse_reflect[2] = stof(vals[3]);

        getline(*inFile, line);
        vals = getSpaceDelimitedWords(line);
        newObj.specular_reflect[0] = stof(vals[1]);
        newObj.specular_reflect[1] = stof(vals[2]);
        newObj.specular_reflect[2] = stof(vals[3]);

        getline(*inFile, line);
        vals = getSpaceDelimitedWords(line);
        newObj.shininess = stof(vals[1]);

        // Fill the transformation vector
        getTransforms(inFile, &newObj);
        // Add the shape to the output
        out->push_back(newObj);
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
    light <xyz point> , <rgb color> , <attenuation>
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
            Camera *cam - The camera info to populate
            vector<Point_Light> lights - Vector of light sources
            map<string, Object*> *originals - Maps object names to the
                object definition.
            map<string, vector<Object> > *out - Maps object names to a
                vector of its transformed copies.

 Returns:   (int) - 0 if successful, nonzero otherwise
*/
int parseScene
(
    char                            *filename,
    Camera                          *cam,
    vector<Point_Light>             *lights,
    map<string, Object*>            *originals,
    vector<Object>                  *out
)
{
    assert(filename);
    assert(originals);
    assert(lights);
    assert(cam);
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
    getOriginalShapes(&inFile, originals);

    // The next line(s) are shapes to copy and transformations to do
    getTransformedShapes(&inFile, originals, out);

    return 0;
}
