#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

#include <Eigen/Dense>

// Externally public functions
void transformMatrix (Eigen::MatrixXd*, std::vector<Eigen::MatrixXd>*);
int parseTransform (char*, std::vector<Eigen::MatrixXd>*);
