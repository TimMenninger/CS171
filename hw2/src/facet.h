/******************************************************************************

 facet.h

 Contains facet struct

 Author: Tim Menninger

******************************************************************************/
#ifndef FACET
#define FACET

#include "camera.h"
#include "light.h"
#include "vertex.h"

/*
 facet

 Struct that defines three vertices that describe the face.  Each vertex is
 an integer index to a list of vertices.  It also describes a vector normal
 to the facet.
*/
typedef struct _facet {
    int v1;
    int v2;
    int v3;

    int n1;
    int n2;
    int n3;

    _facet() : v1 (0), v2 (0), v3 (0), n1 (0), n2 (0), n3 (0) {}
    _facet(int v1, int v2, int v3, int n1, int n2, int n3)
        : v1 (v1), v2 (v2), v3 (v3), n1 (n1), n2 (n2), n3 (n3) {}
    ~_facet() {}

    bool inView(std::vector<int>);

} facet;

#endif // ifndef FACET
