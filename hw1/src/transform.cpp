/******************************************************************************

 transform.cpp

 Contains code that will transform a matrix.

 Author: Tim Menninger

******************************************************************************/

#include "transform.h"

using namespace std;
using namespace Eigen;

/*
 generateMatrix

 For each line, we will create a matrix of 4 dimensions.  This only works for
 3D systems.  This parses the file until either an empty line or the end of
 the file.
    t   translation vector (tx, ty, tz) creates matrix
            [ 1  0  0 tx ]
            [ 0  1  0 ty ]
            [ 0  0  1 tz ]
            [ 0  0  0  1 ]
    r   rotation vector and rotation degrees (rx, ry, rz, theta)
        in radians, where rx, ry, rz define the axis around which the
        object will be rotated counterclockwise by theta radians.
    s   scalar matrix (sx, sy, sz) creates matrix
            [ sx   0   0   0 ]
            [  0  sy   0   0 ]
            [  0   0  sz   0 ]
            [  0   0   0   1 ]

 Arguments: ifstream *openFile - The file being read
 vector<MatrixXd> *matrices - The matrices created from the
 instructions in the file.

 Returns:   (int) - Zero if successful, nonzero otherwise.
*/
int generateMatrix
(
    ifstream            *openFile,
    vector<MatrixXd>    *matrices
)
{
    assert(openFile);
    assert(matrices);

    // The string buffer that will be read
    string line;
    // Read each line and create an element
    while (getline(*openFile, line)) {
        // Will be the matrix that is added next
        MatrixXd m(4, 4);

        // Generate the matrix from the instruction on this line
        if (generateTransform(line, &m) == 0)
            return 0;

        matrices->push_back(m);
    }

    return 0;
}

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
    vector<string> vals;

    string temp;
    stringstream s(transform);
    while (s >> temp)
        vals.push_back(temp);

    // If at an empty line, stop
    if (vals.size() == 0)
        return 0;

    // Faster to convert to float only once
    float x = stof(vals[1]), y = stof(vals[2]), z = stof(vals[3]);
    // Only used in 'r' case
    float sinT, cosT, theta;

    // Handle the possible transform cases
    switch (transform[0]) {
        case 'r':
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
 parseTransformFile

 Parses a file and generates a list of matrices from the instructions in the
 file.

 Arguments: char *filename - The name of the file to parse
            vector<MatrixXd> *matrices - Vector to output list of transform
                matrices.  These will be 4x4 matrices.

 Returns:   (int) - Zero on success, nonzero otherwise
*/
int parseTransformFile
(
    char                *filename,
    vector<MatrixXd>    *matrices
)
{
    assert(filename);
    assert(matrices);

    // Read the file
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "unable to open " << filename << endl;
        return 1;
    }

    int status = generateMatrix(&inFile, matrices);

    inFile.close();

    return status;
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
