#include "transform.h"

using namespace std;
using namespace Eigen;

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
    if (parseTransformFile(argv[1], &transform_data) != 0)
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
