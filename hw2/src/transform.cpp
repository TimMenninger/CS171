/******************************************************************************

 transform.cpp

 Contains code that will transform a matrix.

 Author: Tim Menninger

******************************************************************************/

#include "transform.h"

using namespace std;
using namespace Eigen;

/*
 generateTransform

 Takes a string which is a transformation vector and populates a matrix with
 the 4D transformation matrix that corresponds to it.

 Arguments: string transform - A string representation of the transformation
                vector
            MatrixXd m - The matrix to populate

 Returns:   int - Number of values in string, negative if error
*/
int generateTransform
(
    string              transform,
    MatrixXd            *m
)
{
    // Split the space-separated line into a vector of strings
    vector<string> vals = getSpaceDelimitedWords(transform);

    // If at an empty line, stop
    if (vals.size() == 0)
        return 0;

    // Faster to convert to float only once
    float x = stof(vals[1]), y = stof(vals[2]), z = stof(vals[3]), mag;

    // Only used in 'r' case
    float sinT, cosT, theta;

    // Handle the possible transform cases
    switch (transform[0]) {
        case 'r':
            // Normalize the axis of rotation
            mag = sqrt(x*x + y*y + z*z);
            x /= mag;
            y /= mag;
            z /= mag;

            // Last value of rotation is angle
            theta = stof(vals[4]);
            sinT = sin(theta);
            cosT = cos(theta);

            *m <<    x*x+(1-x*x)*cosT,   x*y*(1-cosT)-z*sinT,   x*z*(1-cosT)+y*sinT, 0,
                  x*y*(1-cosT)+z*sinT,      y*y+(1-y*y)*cosT,   y*z*(1-cosT)-x*sinT, 0,
                  x*z*(1-cosT)-y*sinT,   y*z*(1-cosT)+x*sinT,      z*z+(1-z*z)*cosT, 0,
                                    0,                     0,                     0, 1;
            break;
        case 's':
            // Expect 's' then n dimensions of scalar
            *m << x, 0, 0, 0,
                  0, y, 0, 0,
                  0, 0, z, 0,
                  0, 0, 0, 1;
            break;
        case 't':
            // Should contain 't' then translate in n dimensions
            *m <<  1, 0, 0, x,
                   0, 1, 0, y,
                   0, 0, 1, z,
                   0, 0, 0, 1;
            break;
        default:
            cout << "unable to parse file" << endl;
            return -1;
    }

    return vals.size();
}

/*
 transformMatrix

 Takes a vector of matrix factors and a matrix and multiplies the matrix by
 each of the matrices in the vector, in reverse order.  So if we have matrix
 M and vector { A, B, C } the product will be ABCM, returned in an argued
 matrix.  The matrix argued should be of the same dimension as those in
 the vector, and should be square.

 Arguments: MatrixXd *matrix - A matrix that will be multiplied.  The output
                will be stored in this matrix.
            vector<MatrixXd> *factors - The matrices to multiply by

 Returns:   Nothing
*/
void transformMatrix
(
    MatrixXd            *matrix,
    vector<MatrixXd>    *factors
)
{
    assert(matrix);
    assert(factors);

    // Multiply by all of the matrices in the file.
    vector<MatrixXd>::reverse_iterator it = factors->rbegin();
    for (; it != factors->rend(); ++it) {
        // Multiply matrix by the accumulator
        *matrix = *it * *matrix;
    }
    return;
}
