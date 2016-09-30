#include "objParser.h"

using namespace std;

/*
 main

 Main loop to parse OBJ files and output them.  This outputs all of the read
 files to out.txt
*/
int main
(
    int argc,
    char **argv
)
{
    if (argc < 2) {
        cout << "usage: ./objParser <obj_file1.obj> ..." << endl;
        return 1;
    }
    // Assume that each argument is a valid and existant .OBJ file
    for (int i = 1; i < argc; i++) {
        // parser expects vector buffer
        vector<vertex> vertices;
        vector<facet> facets;
        shape3D shape("", &vertices, &facets);
        int s = parseObjFile(argv[i], &shape);
        if (s)
            return s;

        // Print the shape
        shape.print();
    }

    return 0;
}
