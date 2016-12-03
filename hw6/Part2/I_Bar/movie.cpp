/******************************************************************************

 movie.cpp

 Imports a movie description from a script file for CS171 hw6

 Author: Tim Menninger

******************************************************************************/
#include "movie.h"

using namespace std;
using namespace Eigen;

/*
 Movie::import

 Imports the keyframes for the movie, described in the argued filename.

 Arguments: char *filename - The name of the file to read the movie from.

 Returns:   (int) - 0 on success, nonzero otherwise

 Revisions: 11/30/16 - Tim Menninger: Created
*/
int Movie::import(char *filename) {
    // Read the file
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "unable to open " << filename << endl;
        return 1;
    }

    // Splitting each space-separated line into a vector of strings
    string line;
    vector<string> vals;

    // First line has number of frames
    getline(inFile, line);
    vals = getSpaceDelimitedWords(line);
    this->numFrames = stoi(vals[0]);

    // Before filling frames, make space
    this->firstFrame = NULL;
    Keyframe *kf = NULL;
    Transforms *t;

    // Import all of the keyframes
    while (getline(inFile, line)) {
        // Split the space-separated line into a vector of strings
        vals = getSpaceDelimitedWords(line);
        if (vals.size() == 0)
            break;

        // Keyframe has following form:
        //      Frame #
        //      translation # # #
        //      scale # # #
        //      rotation # # # #
        // If it's the first line of a frame, maek a new keyframe
        if (line[0] == 'F' || line[0] == 'f') {
            if (kf != NULL) {
                kf->next = new Keyframe(stoi(vals[1]));
                kf->next->prev = kf;
                kf = kf->next;
            } else {
                kf = new Keyframe(stoi(vals[1]));
                this->firstFrame = kf;
            }
            // Set transforms so we can fill them
            t = kf->t;
            // Read next line to get transformation
            getline(inFile, line);
            vals = getSpaceDelimitedWords(line);
        }

        // Extract the values from the string
        float x = stof(vals[1]), y = stof(vals[2]), z = stof(vals[3]);
        float rotation[4] = { 0, x, y, z };

        switch (line[0]) {
            case 'R':
            case 'r':
                // Take the rotation and store the quaternion
                rotation[0] = (float) (stoi(vals[4]) % 360) * PI / 180;
                rotationToQuaternion(rotation, t->rotation);
                break;
            case 'T':
            case 't':
                // Fill the translation
                t->translation[0] = x;
                t->translation[1] = y;
                t->translation[2] = z;
                break;
            case 'S':
            case 's':
                // Fill in scaling vector
                t->scaling[0] = x;
                t->scaling[1] = y;
                t->scaling[2] = z;
                break;
            default:
                cout << "unable to parse file" << endl;
                return 1;
        }
    }

    // Point the first frame and last frame to each other
    if (kf != NULL) {
        kf->next = this->firstFrame;
        this->firstFrame->prev = kf;
    }

    // Start movie at first (0'th) frame
    this->currIdx = 0;
    this->currFrame = this->firstFrame; // prev so the first next frame is the start

    return 0;
}

/*
 Movie::nextTransform

 Given the current transform and the previous and next keyframes, computes what
 the transformation should be for the next frame.  This uses Catmull-Rom
 splines to compute the next location.  We compute:
        f(.u.) = .u. .B. .p.
 Here, .u. is the vector [1, u, u^2, u^3], where u is in [0, 1), computed from
 how many frames we are from the previous and next keyframes.  The vector .p.
 is [p_{i-1}, p_{i}, p_{i+1}, p_{i+2}], where p_{i} is of the last keyframe
 before or at the same time as the current frame.  .B. is the inverse
 constraint matrix.  The tension parameter is 0 (by definition of Catmull-Rom
 splines), so this matrix is
        [  0    1    0    0   ]
        [ -0.5  0    0.5  0   ]
        [  1   -2.5  2   -0.5 ]
        [ -0.5  1.5 -1.5  0.5 ]
 This must be computed in each dimension separately.

 Arguments: Transform *t - The transform that is filled with the transformation
                of the next frame.

 Returns:   Nothing.

 Revisions: 12/01/16 - Tim Menninger: Created
*/
void Movie::nextTransform(Transforms *t) {
    assert(this->firstFrame);
    // Fills constraint matrix used in Catmull-Rom spline computing
    fillConstraintMatrix();

    // Shorthand
    int lastFrame = this->numFrames - 1;

    // Keyframes i-1 through i+2, with i-1 ==> 0 and i+2 ==> 3
    Keyframe *kf0, *kf1, *kf2, *kf3;
    kf1 = this->currFrame;
    kf0 = kf1->prev;
    kf2 = kf1->next;
    kf3 = kf2->next;

    // Use current keyframe index and next keyframe index to compute u.  Make
    // sure that if the next frame is index 0, that we use the number of frames
    float uFloat = (float) (this->currIdx - kf1->idx) /
        (float) ((kf2->idx == 0 ? lastFrame : kf2->idx) - kf1->idx);
    MatrixXd u(1, 4);
    u << 1, uFloat, uFloat*uFloat, uFloat*uFloat*uFloat;

    // Get the corresponding transforms for each of the keyframes.  Because
    // the Transforms struct is just 10 floats, we will treat each like an
    // array of 10 floats.  Transforms are numbered as keyframes were.
    float *t0, *t1, *t2, *t3, *out;
    t0  = (float *) kf0->t;
    t1  = (float *) kf1->t;
    t2  = (float *) kf2->t;
    t3  = (float *) kf3->t;
    out = (float *) t;

    // Perform the same operation on all 10 floats, for each, filling the
    // result into the parameter Transforms
    for (int i = 0; i < 10; ++i) {
        // Create our p vector from our keyframe Transforms
        MatrixXd p(4, 1);
        p << t0[i], t1[i], t2[i], t3[i];

        // Fill output
        out[i] = (u * B * p)(0);
    }
    // Normalize quaternion
    float norm = sqrt(t->rotation[0]*t->rotation[0] +
                      t->rotation[1]*t->rotation[1] +
                      t->rotation[2]*t->rotation[2] +
                      t->rotation[3]*t->rotation[3]);
    t->rotation[0] /= norm;
    t->rotation[1] /= norm;
    t->rotation[2] /= norm;
    t->rotation[3] /= norm;

    // Advance the movie
    this->currIdx = (this->currIdx == lastFrame ? 0 : this->currIdx + 1);
    if (this->currIdx == this->currFrame->next->idx)
        this->currFrame = kf2;
}
