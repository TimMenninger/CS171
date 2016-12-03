/******************************************************************************

 utils.cpp

 Contains utilities to help the readability and functionality of transformation
 code.

 Author: Tim Menninger

******************************************************************************/
#include "utils.h"

using namespace std;

/*
 getSpaceDelimitedWords

 Takes a line and returns a vector of words in that line, which are assumed
 to be separated by any amount of whitespace.

 Arguments: string line - The line to parse

 Returns:   vector<string> - A vector of words in the argued line
*/
vector<string> getSpaceDelimitedWords
(
    string line
)
{
    // Split the space-separated line into a vector of strings
    vector<string> vals;
    string temp;
    stringstream s(line);
    while (s >> temp)
        vals.push_back(temp);

    return vals;
}

/*
 rotationToQuaternion

 Converts a rotation vector (theta, x, y, z) to a quaternion.

 Arguments: float *rotation - The rotation vector
            float *quaternion - Where the quaternion is stored

 Returns:   Nothing.

 Revisions: 12/01/16 - Tim Menninger: Created
*/
void rotationToQuaternion(float *rotation, float *quaternion) {
    // Assumes rotation is [theta, x, y, z] with theta in radians
    float theta = rotation[0];
    quaternion[0] = cos(theta / 2);
    quaternion[1] = rotation[1] * sin(theta / 2);
    quaternion[2] = rotation[2] * sin(theta / 2);
    quaternion[3] = rotation[3] * sin(theta / 2);

    // Normalize
    float norm = sqrt(quaternion[0]*quaternion[0] +
                      quaternion[1]*quaternion[1] +
                      quaternion[2]*quaternion[2] +
                      quaternion[3]*quaternion[3]);
    quaternion[0] /= norm;
    quaternion[1] /= norm;
    quaternion[2] /= norm;
    quaternion[3] /= norm;
}

/*
 quaternionToRotation

 Converts a quaternion to a rotation vector (theta, x, y, z).

 Arguments: float *quaternion - The quaternion to convert
            float *rotation - Where to store the rotation vector

 Returns:   Nothing.

 Revisions: 12/01/16 - Tim Menninger: Created
*/
void quaternionToRotation(float *quaternion, float *rotation) {
    // Puts theta in radians into rotation [theta, x, y, z]
    float qs = quaternion[0];
    rotation[0] = 2 * acos(qs);
    if (qs != 1) {
        // Don't want to divide by 0
        rotation[1] = quaternion[1] / sqrt(1 - qs*qs);
        rotation[2] = quaternion[2] / sqrt(1 - qs*qs);
        rotation[3] = quaternion[3] / sqrt(1 - qs*qs);
    } else {
        // Arbitrary axis because theta is 0
        rotation[1] = 1;
        rotation[2] = 0;
        rotation[3] = 0;
    }

    // Normalize
    float norm = sqrt(rotation[1]*rotation[1] +
                      rotation[2]*rotation[2] +
                      rotation[3]*rotation[3]);
    rotation[1] /= norm;
    rotation[2] /= norm;
    rotation[3] /= norm;
}
