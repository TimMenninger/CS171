/******************************************************************************

 I_bar.h

 Contains public function declarations from I_bar.cpp.

 Author: Tim Menninger

******************************************************************************/
#ifndef IBAR
#define IBAR

#include <vector>
#include <cmath>

#include "movie.h"
#include "utils.h"

#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>
#else
    #include <GL/glew.h>
    #include <GL/glut.h>
#endif

// Light for the system
static void initLight(Point_Light *light) {
    light->position[0] = 0;
    light->position[0] = 0;
    light->position[0] = 40;
    light->position[0] = 1;

    light->color[0] = 1;
    light->color[1] = 1;
    light->color[2] = 1;

    light->attenuation_k = 0.5;
}
// Camera used (hard coded, given from hw6 specs)
static void initCamera(Camera *cam) {
    cam->cam_position[0] = 0;
    cam->cam_position[1] = 0;
    cam->cam_position[2] = 40;

    cam->cam_orientation_axis[0] = 0;
    cam->cam_orientation_axis[1] = 0;
    cam->cam_orientation_axis[2] = 0;
    cam->cam_orientation_angle   = 0;

    cam->near_param   = 1.0;
    cam->far_param    = 60.0;
    cam->left_param   = -1.0;
    cam->right_param  = 1.0;
    cam->top_param    = 1.0;
    cam->bottom_param = -1.0;
}

#endif // ifndef IBAR
