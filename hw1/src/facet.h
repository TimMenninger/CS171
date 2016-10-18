#ifndef FACET
#define FACET

/*
 facet

 Struct that defines three vertices that describe the face.  Each vertex is
 an integer index to a list of vertices.
*/
typedef struct _facet {
    int v1;
    int v2;
    int v3;

    _facet() : v1 (0), v2 (0), v3 (0) {}
    _facet(int v1, int v2, int v3) : v1 (v1), v2 (v2), v3 (v3) {}
    ~_facet() {}
} facet;

#endif // ifndef FACET
