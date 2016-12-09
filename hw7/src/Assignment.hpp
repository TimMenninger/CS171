#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include <vector>
#include <cmath>
#include <Eigen/Dense>

// Threshold before stopping Newton method iteration
#define THRESHOLD 0.01
// Maximum number of iterations on Newton method (to avoid infinite loop)
#define MAX_NEWTON 50
// Maximum recursion depth when searching for primitives
#define MAX_RECURSION 1000
// We need to slow down the g/g' term to avoid diverging so easily
#define SLOWDOWN 0.1

class Camera;
class Scene;

namespace Assignment {
    void drawIOTest();
    void drawIntersectTest(Camera *camera);
    void raytrace(Camera camera, Scene scene);
};

#endif
