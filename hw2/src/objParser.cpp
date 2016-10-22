/******************************************************************************

 objParser.cpp

 Parses OBJ files and creates a shape object containing a vector of its
 vertices and faces.

 Usage: ./objParser

 Author: Tim Menninger

******************************************************************************/

#include "objParser.h"

using namespace std;

/*
 parseObjFile

 Opens the argued OBJ file and creates a vertex for every vertex and a facet
 for every face in the file.  This assumes that the OBJ file is such that
 every line either contains v <float> <float> <float> or f <int> <int> <int>.
 If this is not the case, the behavior is undefined.  It then writes the
 contents to the argued output file with the name of the input file ahead
 of it.

 Arguments: char *filename - The name of the OBJ file to read
            shape3D *shape - The output shape

 Returns:   (int) - Status, 0 implies success. nonzero otherwise.
*/
int parseObjFile
(
    string          filename,
    shape3D         *shape
)
{
    assert(shape);
    // Set the filename
    string fn(filename);
    shape->name = fn.substr(0, fn.length()-FEXT_LEN);
    // Open the file
    ifstream inFile(fn);
    if (!inFile.is_open()) {
        cout << "unable to open " << fn << endl;
        return 1;
    }

    // To make facets easier, put an empty vertex at beginning of vertices
    //    so we can one-index them.   Same for surface normals
    shape->clear();

    // Instantiate a vertex and facet to fill before each push_back
    vertex ver;
    facet fac;
    normal vn;

    // The string buffer that will be read
    string line;
    // Read each line and create an element
    while (getline(inFile, line)) {
        // Split the space-separated line into a vector of strings
        vector<string> vals;
        string temp;
        stringstream s(line);
        while (s >> temp)
            vals.push_back(temp);

        // Handle the line according to its identifier (vertex or facet)
        if (vals[0] == "v") {
            // Is a vertex -- vals contains 'v' then 3 floats
            ver = vertex(stof(vals[1]), stof(vals[2]), stof(vals[3]));
            shape->vertices->push_back(ver);
        } else if (vals[0] == "vn") {
            vn = normal(stof(vals[1]), stof(vals[2]), stof(vals[3]));
            shape->normals->push_back(vn);
        } else if (vals[0] == "f") {
            int vertices[3];
            int normals[3];
            // Is a facet -- vals contains 'f' then 3 ints//int values
            for (int i = 0; i < 3; ++i) {
                int delimiterIdx = vals[i+1].find("//");
                vertices[i] = stoi(vals[i+1].substr(0, delimiterIdx));
                normals[i] = stoi(vals[i+1].substr(delimiterIdx+2));
            }
            fac = facet(vertices[0], vertices[1], vertices[2],
                        normals[0], normals[1], normals[2]);
            shape->facets->push_back(fac);
        } else {
            cout << "error parsing " << fn << endl;
            inFile.close();
            return 1;
        }
    }
    inFile.close();

    return 0;
}
// Different definition for char* input instead of string
int parseObjFile
(
    char    *filename,
    shape3D *shape
)
{
    assert(filename);
    assert(shape);
    return parseObjFile(string(filename), shape);
}
