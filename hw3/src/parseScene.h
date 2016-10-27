/******************************************************************************

 parseScene.h

 Contains public function declarations from parseScene.cpp.  Much of this was
 adapted from the opengl demo code supplied by CS/CNS 171 this Fall, 2016.

 Author: Tim Menninger

******************************************************************************/

/* The following 2 headers contain all the main functions, data structures, and
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


#include <map>

#include "objParser.h"
#include "utils.h"

#define OBJ_DIR "data/"

/* You will almost always want to include the math library. For those that do
 * not know, the '_USE_MATH_DEFINES' line allows you to use the syntax 'M_PI'
 * to represent pi to double precision in C++. OpenGL works in degrees for
 * angles, so converting between degrees and radians is a common task when
 * working in OpenGL.
 *
 * Besides the use of 'M_PI', the trigometric functions also show up a lot in
 * graphics computations.
 */
#include <math.h>
#define _USE_MATH_DEFINES

/* iostream and vector are standard libraries that are just generally useful.
 */
#include <iostream>
#include <vector>

#ifndef PARSESCENE
#define PARSESCENE

///////////////////////////////////////////////////////////////////////////////////////////////////

/* The following structs do not involve OpenGL, but they are useful ways to
 * store information needed for rendering,
 *
 * After Assignment 2, the 3D shaded surface renderer assignment, you should
 * have a fairly intuitive understanding of what these structs represent.
 */


/* The following struct is used for representing a point light.
 *
 * Note that the position is represented in homogeneous coordinates rather than
 * the simple Cartesian coordinates that we would normally use. This is because
 * OpenGL requires us to specify a w-coordinate when we specify the positions
 * of our point lights. We specify the positions in the 'set_lights' function.
 */
struct Point_Light
{
    /* Index 0 has the x-coordinate
     * Index 1 has the y-coordinate
     * Index 2 has the z-coordinate
     * Index 3 has the w-coordinate
     */
    float position[4];

    /* Index 0 has the r-component
     * Index 1 has the g-component
     * Index 2 has the b-component
     */
    float color[3];

    /* This is our 'k' factor for attenuation as discussed in the lecture notes
     * and extra credit of Assignment 2.
     */
    float attenuation_k;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/* The following are the typical camera specifications and parameters. In
 * general, it is a better idea to keep all this information in a camera
 * struct, like how we have been doing it in Assignemtns 1 and 2. However,
 * if you only have one camera for the scene, and all your code is in one
 * file (like this one), then it is sometimes more convenient to just have
 * all the camera specifications and parameters as global variables.
 */

struct Camera {
    /* Index 0 has the x-coordinate
     * Index 1 has the y-coordinate
     * Index 2 has the z-coordinate
     */
    float cam_position[3];
    float cam_orientation_axis[3];

    /* Angle in degrees.
     */
    float cam_orientation_angle;

    float near_param, far_param,
          left_param, right_param,
          top_param, bottom_param;
};

// Externally public functions
int parseScene (char*, Camera*, std::vector<Point_Light>*, std::map<std::string, Object*>*, std::vector<Object>*);

#endif // ifndef PARSESCENE
