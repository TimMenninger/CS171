#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include <vector>
#include <cmath>
#include <Eigen/Dense>

#define THRESHOLD 0.05
#define MAX_RECURSION 1000

class Camera;
class Scene;

namespace Assignment {
    void drawIOTest();
    void drawIntersectTest(Camera *camera);
    void raytrace(Camera camera, Scene scene);
};

#endif
