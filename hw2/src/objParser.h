/******************************************************************************

 objParser.h

 Contains public functions from objParser.cpp.

 Author: Tim Menninger

******************************************************************************/
#ifndef OBJPARSER
#define OBJPARSER

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>

#include "geom.h"
#include "shape3d.h"


#define FEXT_LEN        4         // Number of characters in OBJ file extension


// Externally public functions
int parseObjFile (char*, shape3D*);
int parseObjFile (std::string, shape3D*);

#endif // ifndef OBJPARSER
