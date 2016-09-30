#include "transformObjs.h"

using namespace std;
using namespace Eigen;


/*
 getOriginalShapes

 Takes an open file, reads a line containing an object name and filepath,
 and fills a shape with its contents.  It loads the shape into a map indexed
 by the name.

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
    order->clear();
    string line;
    while (getline(*inFile, line)) {
        // Split the space-separated line into a vector of strings
        vector<string> vals;
        string temp;
        stringstream s(line);
        while (s >> temp)
            vals.push_back(temp);

        // An empty line separates the files from the transforms
        if (vals.size() == 0) {
            return;
        }

        // The first value is the name, the second is the file
        shape3D *orig = new shape3D();
        orig->vertices = new vector<vertex>;
        orig->facets = new vector<facet>;
        // Parse the obj file we obtained from this description
        parseObjFile(vals[1], orig);
        orig->name = vals[0];
        // Save the object
        order->push_back(vals[0]);
        (*originals)[vals[0]] = orig;
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
 parseShapeTransforms

 Iterates through lines in the file with the argued name.  It expects the first
 lines to be shape names and filepaths.  It reads those and saves them.  Then,
 an empty line, then a set of transformations.  Each transformation is
 separated by an empty line and has the form of <shape_name>\n<transformations>
 with each transformation on a new line.  It fills a vector with the order
 objects were read, a map of names to original objects, and a map of names
 to a vector of copies of that original object.

 Arguments: char *filename - The name of the file to parse
            vector<string> - A vector of object names in the order they were
                read.
            map<string, shape3D*> *originals - Maps object names to the
                object definition.
            map<string, vector<shape3D> > *out - Maps object names to a
                vector of its transformed copies.

 Returns:   (int) - 0 if successful, nonzero otherwise
*/
int parseShapeTransforms
(
    char                            *filename,
    vector<string>                  *order,
    map<string, shape3D*>           *originals,
    map<string, vector<shape3D> >   *out
)
{
    // Read the file
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "unable to open " << filename << endl;
        return 1;
    }

    // The first line(s) are objects and names for those objects
    getOriginalShapes(&inFile, order, originals);

    // The next sets of lines each describe a transformation on an object
    string line;
    while (getline(inFile, line)) {
        // Create a transformation matrix from the instructions
        MatrixXd transMatrix = MatrixXd::Identity(4, 4);
        vector<MatrixXd> matrices;
        generateMatrices(&inFile, &matrices);
        transformMatrix(&transMatrix, &matrices);

        // Transform the shape
        shape3D transformed;
        transformed.vertices = new vector<vertex>;
        transformed.facets = new vector<facet>;
        string objName = line.substr(0, line.length());
        shape3D *origShape = (*originals)[objName];
        transformShape(origShape, &transMatrix, &transformed);
        // Give the name and copy number to the transformed shape
        ostringstream copyNumber;
        copyNumber << (*out)[objName].size() + 1;
        transformed.name = objName + "_copy" + copyNumber.str();

        // Add the shape to the output
        (*out)[objName].push_back(transformed);
    }

    return 0;
}

/*
 printShapes

 Prints the shapes according to the CS171 hw0 FA2016 specification.

 Arguments: vector<string> *order - The order in which to print the shapes,
                which matches the order they were read from the file
            map<string, shape3D*> *originals - All of the original objects,
                as they were in their OBJ files, indexed by name
            map<string, vector<shape3D> > *transforms - The transformed copies
                of the objects, indexed by the name of the original object

 Returns:   Nothing.
*/
void printShapes
(
    vector<string>                  *order,
    map<string, shape3D*>           *originals,
    map<string, vector<shape3D> >   *transforms
)
{
    // Print all of the originals, keeping a list of the order they're printed
    // to make it easier to look at.
    vector<string>::iterator it = order->begin();
    for (; it != order->end(); ++it) {
        (*originals)[*it]->print();
    }

    // Print all of the transformations
    it = order->begin();
    for (; it != order->end(); ++it) {
        // Get the list of copies
        vector<shape3D> shapes = (*transforms)[*it];
        vector<shape3D>::iterator shape = shapes.begin();
        // Print each copy
        for (; shape != shapes.end(); ++shape) {
            shape->print();
        }
    }
}

/*
 cleanupShapes

 Takes the map of original shapes and its copies and frees allocated memory.
 This function is very specific to the operations done in this file.

 Arguments: map<string, shape3D*> *shapes - Frees all of the shapes and their
                vector pointers.
            map<string, vector<shape3D> > *shapesVec - Frees only the vector
                pointers (because the shapes aren't pointers)

 Returns:   Nothing.
 */
void cleanupShapes
(
    map<string, shape3D*>           *shapes,
    map<string, vector<shape3D> >   *shapesVec
)
{
    // Delete the shapes in the shapes map
    map<string, shape3D*>::iterator it = shapes->begin();
    for (; it != shapes->end(); ++it) {
        delete it->second->vertices;
        delete it->second->facets;
        delete it->second;
    }

    // Delete the vector pointers in each shape in each vector
    map<string, vector<shape3D> >::iterator vec = shapesVec->begin();
    for (; vec != shapesVec->end(); ++vec) {
        vector<shape3D>::iterator shape = vec->second.begin();
        for (; shape != vec->second.end(); ++shape) {
            delete shape->vertices;
            delete shape->facets;
        }
    }
}
