/******************************************************************************

 transform.cpp

 Contains code that will transform a matrix.

 Author: Tim Menninger

******************************************************************************/

#include "transform.h"

using namespace std;
using namespace Eigen;

/*
 parseTransform

 For each line, we will create a matrix of 4 dimensions.  This only works for
 3D systems.
      t   translation vector (tx, ty, tz) creates matrix
                  [ 1  0  0 tx ]
                  [ 0  1  0 ty ]
                  [ 0  0  1 tz ]
                  [ 0  0  0  1 ]
      r   rotation vector and rotation degrees (rx, ry, rz, theta)
          in radians, where rx, ry, rz is in {0, 1} and (rx, ry, rz) contains
          exactly two zeroes.  This is true for any number of dimensions.
          The resultant rotation matrix will rotate just the two zero
          dimensions.  If the vector does not satisfy these conditions,
          the behavior is undefined.  The matrix will be 4D but rotate
          only in 2D.  For example, (1, 0, 0, t) gives
                  [       1       0       0       0]
                  [       0  cos(t) -sin(t)       0]
                  [       0  sin(t)  cos(t)       0]
                  [       0       0       0       1]
      s   scalar matrix (sx, sy, sz) creates matrix
                  [ sx   0   0   0 ]
                  [  0  sy   0   0 ]
                  [  0   0  sz   0 ]
                  [  0   0   0   1 ]

 Arguments: char *filename - The name of the file to parse
            vector<MatrixXd> *matrices - Vector to output list of transform
                matrices.  These will be 4x4 matrices.

 Returns:   (int) - Zero on success, nonzero otherwise
*/
int parseTransform
(
    char                *filename,
    vector<MatrixXd>    *matrices
)
{
    // Read the file
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "unable to open " << filename << endl;
        return 1;
    }

    // The string buffer that will be read
    string line;
    // Read each line and create an element
    while (getline(inFile, line)) {
        // Split the space-separated line into a vector of strings
        vector<string> vals;
        string temp;
        stringstream s(line);
        while (s >> temp)
            vals.push_back(temp);

        // Will be the matrix that is added next
        MatrixXd m(4, 4);

        // Faster to convert to float only once
        float x = stof(vals[1]), y = stof(vals[2]), z = stof(vals[3]);
        // Only used in 'r' case
        float sinT, cosT, theta;

        // Handle the possible transform cases
        switch (line[0]) {
            case 'r':
                // Last value of rotation is angle
                theta = stof(vals[4]);
                x = (x == 0 ? 1 : 0);
                y = (y == 0 ? 1 : 0);
                z = (z == 0 ? 1 : 0);
                sinT = sin(theta);
                cosT = cos(theta);

                m << x*cosT + y*z,   -1*x*y*sinT,   -1*x*z*sinT, 0,
                         x*y*sinT,  y*cosT + x*z,   -1*y*z*sinT, 0,
                         x*z*sinT,      y*z*sinT,  z*cosT + x*y, 0,
                                0,             0,             0, 1;
                break;
            case 's':
                // Expect 's' then n dimensions of scalar
                m << x, 0, 0, 0,
                     0, y, 0, 0,
                     0, 0, z, 0,
                     0, 0, 0, 1;
                break;
            case 't':
                // Should contain 't' then translate in n dimensions
                m <<  1, 0, 0, x,
                      0, 1, 0, y,
                      0, 0, 1, z,
                      0, 0, 0, 1;
                break;
            default:
                cout << "unable to parse " << filename << endl;
                inFile.close();
                return 1;
        }

        matrices->push_back(m);
    }

    inFile.close();

    return 0;
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
    // Multiply by all of the matrices in the file.
    vector<MatrixXd>::reverse_iterator it = factors->rbegin();
    for (; it != factors->rend(); ++it) {
        // Multiply matrix by the accumulator
        *matrix = *it * *matrix;
    }
    return;
}

/*
 main

 Creates a transformation matrix given the input file of transform
 instructions.  This only works in 3 dimensions.
*/
int main(int argc, char **argv) {
    // Takes exactly one argument
    if (argc != 2) {
        cout << "usage: ./transform <transform_data_file>";
        return 1;
    }

    vector<MatrixXd> transform_data;
    if (parseTransform(argv[1], &transform_data) != 0)
        return 1;

    MatrixXd m(4, 4);
    m << 1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 0;
    transformMatrix(&m, &transform_data);

    // Invert the transform matrix
    MatrixXd m_inv = m.topLeftCorner(3, 3).inverse();

    // Output the transform matrix
    cout << m_inv << endl;
    return 0;
}
