/******************************************************************************

 objParser.cpp

 Parses OBJ files and creates an object containing a vector of its
 vertices and normals.

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
            Object *obj - The output object

 Returns:   (int) - Status, 0 implies success. nonzero otherwise.
*/
int parseObjFile
(
    string          filename,
    Object          *obj
)
{
    assert(obj);
    // Set the filename
    string fn(filename);
    obj->name = fn.substr(0, fn.length()-FEXT_LEN);
    // Open the file
    ifstream inFile(fn);
    if (!inFile.is_open()) {
        cout << "unable to open " << fn << endl;
        return 1;
    }

    // Initiate vectors that will contain all vertices and normals.  Then, when
    // we populate the vectors in the Object struct, we will index from these.
    vector<Triple> vertices;
    vertices.push_back(Triple(0, 0, 0));
    vector<Triple> normals;
    normals.push_back(Triple(0, 0, 0));

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
            Triple ver(stof(vals[1]), stof(vals[2]), stof(vals[3]));
            vertices.push_back(ver);
        } else if (vals[0] == "vn") {
            Triple vn(stof(vals[1]), stof(vals[2]), stof(vals[3]));
            normals.push_back(vn);
        } else if (vals[0] == "f") {
            // Is a facet -- vals contains 'f' then 3 ints//int values
            for (int i = 0; i < 3; ++i) {
                int delimiterIdx = vals[i+1].find("//");
                obj->vertex_buffer.push_back(
                    vertices[stoi(vals[i+1].substr(0, delimiterIdx))]);
                obj->normal_buffer.push_back(
                    normals[stoi(vals[i+1].substr(delimiterIdx+2))]);
            }
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
    Object  *obj
)
{
    assert(filename);
    assert(obj);
    return parseObjFile(string(filename), obj);
}
