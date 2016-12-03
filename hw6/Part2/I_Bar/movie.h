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
#include <assert.h>
#include <math.h>

#include "../Eigen/Dense"

struct Keyframe {
    int         idx;
    Transforms  *t;
    Keyframe    *prev;
    Keyframe    *next;

    Keyframe(int idx) : idx(idx), t(new Transforms), next(NULL), prev(NULL) {}
};

struct Movie {
    // Values about the movie as a whole
    int         numFrames;
    Keyframe    *firstFrame;
    // Values about where we are in the movie
    int         currIdx;
    Keyframe    *currFrame;

    Movie() : firstFrame(NULL), currFrame(NULL) {}
    ~Movie() {
        Keyframe *kf = firstFrame;
        do {
            Keyframe *temp = kf->next;
            delete kf->t;
            delete kf;
            kf = temp;
        } while (kf != firstFrame);
    }

    int import(char *fn);
    void nextTransform(Transforms *t);
};

// When we're doing Catmull-Rom splines, we always have the same inverse
// constraing matrix, so define it here for less computation
static Eigen::MatrixXd B(4, 4);
static void fillConstraintMatrix() {
    B <<    0,    1,    0,    0,
         -0.5,    0,  0.5,    0,
            1, -2.5,    2, -0.5,
         -0.5,  1.5, -1.5,  0.5;
}

#endif // ifndef MOVIE
