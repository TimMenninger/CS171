/******************************************************************************

 smooth.h

 Contains function headers, macros and includes for smooth.cpp

 Author: Tim Menninger

******************************************************************************/

#ifndef SMOOTH
#define SMOOTH

/* The following headers contain all the main functions, data structures, and
 * variables that allow for OpenGL development.
 */
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>
#else
    #include <GL/glew.h>
    #include <GL/glut.h>
#endif

#include "parseScene.h"
#include "laplace.h"

#endif // ifndef SMOOTH
