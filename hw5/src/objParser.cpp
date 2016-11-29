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
 Object::fillBuffers

 Fills buffers with normals and vertices for OpenGL to use in rendering
 frames.

 Arguments: vector<HEF*> *hefs - Vector of faces that we use to find
                information on edges and vertices.

 Returns:   Nothing.

 Revisions: 11/28/16 - Tim Menninger: Created
*/
void Object::fillBuffers(vector<HEF*> *hefs) {
    // If NULL, use the object's vector
    if (!hefs)
        hefs = this->hefs;

    // Clear first
    this->vertex_buffer.clear();
    this->normal_buffer.clear();

    int numFaces = hefs->size();
    for (int i = 0; i < numFaces; ++i) {
        HEF *face = hefs->at(i);
        this->vertex_buffer.push_back(face->edge->vertex->toVec3f());
        this->normal_buffer.push_back(face->edge->vertex->normal);
        this->vertex_buffer.push_back(face->edge->next->vertex->toVec3f());
        this->normal_buffer.push_back(face->edge->next->vertex->normal);
        this->vertex_buffer.push_back(face->edge->next->next->vertex->toVec3f());
        this->normal_buffer.push_back(face->edge->next->next->vertex->normal);
    }
}
void Object::fillBuffers() {
    this->fillBuffers(NULL);
}

/*
 fillNormals

 Fills normals in the halfedge vertices in the argued vector

 Arguments: vector<HEV*> *hevs - Vector of halfedge vertices that is used to
                compute vertex normals, and then to store said normals.

 Returns:   Nothing.

 Revisions: 11/28/16 - Tim Menninger: Created
*/
void fillNormals(vector<HEV*> *hevs) {
    HEV *hev;
    // Iterate over all vertices
    int numVertices = hevs->size();
    for (int i = 1; i < numVertices; ++i) {
        hev = hevs->at(i);

        // The normal will accumulate data from other faces.
        hev->normal = Vec3f(0, 0, 0);
        // Get the halfedge out of the vertex
        HE *he = hev->out;
        do {
            HEF *face = he->face;

            // Get three vertices describing this face.
            Vec3f v1 = face->edge->vertex->toVec3f();
            Vec3f v2 = face->edge->next->vertex->toVec3f();
            Vec3f v3 = face->edge->next->next->vertex->toVec3f();

            // Normal is (v2-v1) cross (v3-v1)
            Vec3f face_normal = (v2-v1).cross(v3-v1);
            double face_area = computeTriangleArea(v1, v2, v3);
            face_normal.normalize();
            hev->normal += face_normal * face_area;

            // he->flip gets other halfedge corresponding to he
            // ->next gets next edge that hasn't been traversed and is incident
            //      to hev
            he = he->flip->next;
        } while (he != hev->out);
        hev->normal.normalize();
    }
}
void Object::fillNormals() {
    ::fillNormals(this->hevs);
}

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

    // Shorthand
    vector<Vertex*> *vertices = obj->mesh.vertices;
    vector<Face*>   *faces    = obj->mesh.faces;
    if (!vertices || !faces) {
        cout << "unable to allocate memory" << endl;
        exit(1);
    }

    vertices->clear();
    faces->clear();

    // Filler because vertices are 1-indexed
    vertices->push_back(NULL);

    // The string buffer that will be read
    string line;
    // Read each line and create an element
    while (getline(inFile, line)) {
        // Split the space-separated line into a vector of strings
        vector<string> l;
        string temp;
        stringstream s(line);
        while (s >> temp)
            l.push_back(temp);

        // Handle the line according to its identifier (vertex or facet)
        if (l[0] == "v") {
            // Is a vertex -- l contains 'v' then 3 floats
            Vertex *v = new Vertex(stof(l[1]), stof(l[2]), stof(l[3]));
            vertices->push_back(v);
        } else if (l[0] == "f") {
            // Is a facet -- l contains 'f' then 3 int values
            Face *f = new Face(stoi(l[1]), stoi(l[2]), stoi(l[3]));
            faces->push_back(f);
        } else {
            cout << "error parsing " << fn << endl;
            inFile.close();
            return 1;
        }
    }
    inFile.close();

    // Construct halfedge data structure for object
    build_HE(&(obj->mesh), obj->hevs, obj->hefs);
    obj->fillNormals();
    obj->fillBuffers();

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
