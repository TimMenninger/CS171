#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <iomanip>

#include "../Eigen/Dense"

#define KF_BUF_SIZE 5
#define MAX_THREADS 6
#define NUM_DIGITS  2 // Number of digits in number in filenames

struct Vertex {
    double x, y, z;

    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
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
