#ifndef TRANSFORM
#define TRANSFORM

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <assert.h>

#include <Eigen/Dense>
#include "utils.h"

// Externally public functions
void transformMatrix (Eigen::MatrixXd*, std::vector<Eigen::MatrixXd>*);
int generateMatrices (std::ifstream*, std::vector<Eigen::MatrixXd>*);
int parseTransformFile (char*, std::vector<Eigen::MatrixXd>*);

#endif // ifndef TRANSFORM
