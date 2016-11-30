/******************************************************************************

 movie.cpp

 Imports a movie description from a script file for CS171 hw6

 Author: Tim Menninger

******************************************************************************/
#include "movie.h"

using namespace std;

/*
 importMovie

 Imports the keyframes for the movie, described in the argued filename.

 Arguments: char *filename - The name of the file to read the movie from.
            Movie *movie - The movie struct to fill

 Returns:   (int) - 0 on success, nonzero otherwise

 Revisions: 11/30/16 - Tim Menninger: Created
*/
int importMovie(char *filename, Movie *movie) {
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
    movie->numFrames = stoi(vals[0]);

    // Before filling frames, make space
    movie->firstFrame = NULL;
    Keyframe *kf = NULL;
    Transforms t;

    // Import all of the keyframes
    movie->numKeyframes = 0;
    while (getline(inFile, line)) {
        // Keyframe has following form:
        //      Frame #
        //      translation # # #
        //      scale # # #
        //      rotation # # # #
        // If it's the first line of a frame, maek a new keyframe
        if (line[0] == 'F' || line[0] == 'f') {
            if (kf != NULL) {
                kf->nextFrame = new Keyframe();
                kf = kf->nextFrame;
            } else {
                kf = new Keyframe();
                movie->firstFrame = kf;
            }
            // Set transforms so we can fill them
            t = kf->transforms;
            // Read next line to get transformation
            getline(inFile, line);
        }

        // Split the space-separated line into a vector of strings
        vals = getSpaceDelimitedWords(line);

        // Extract the values from the string
        float x = stof(vals[1]), y = stof(vals[2]), z = stof(vals[3]), mag;
        // Will be what gets filled with the transform
        float *transformation;

        switch (line[0]) {
            case 'R':
            case 'r':
                // In rotation, we also have a degrees to rotate
                transformation = t.rotation;
                mag = sqrt(x*x + y*y + z*z);
                x /= mag;
                y /= mag;
                z /= mag;
                t.rotation_angle = stof(vals[4]) * 180 / PI;
                break;
            case 'T':
            case 't':
                // Fill the translation
                transformation = t.translation;
                break;
            case 'S':
            case 's':
                // Fill in scaling vector
                transformation = t.scaling;
                break;
            default:
                cout << "unable to parse file" << endl;
                return 1;
        }
        // Fill in the transformation with the row in the file.
        transformation[0] = x;
        transformation[1] = y;
        transformation[2] = z;
    }

    return 0;
}
