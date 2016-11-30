/******************************************************************************

 movie.h

 Structs that define an animation

 Author: Tim Menninger

******************************************************************************/
#ifndef MOVIE
#define MOVIE

#include "scene.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Eigen/Dense"

struct Keyframe {
    int         frameNumber;
    Transforms  transforms;
    Keyframe    *nextFrame;

    Keyframe() : nextFrame(NULL) {}
};

struct Movie {
    int         numFrames;
    int         numKeyframes;
    Keyframe    *firstFrame;
};

int importMovie(char *filename, Movie *movie);

#endif // ifndef MOVIE
