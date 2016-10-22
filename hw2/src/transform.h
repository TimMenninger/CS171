/******************************************************************************

 transform.h

 Contains public function declarations from transform.cpp.

 Author: Tim Menninger

******************************************************************************/
#ifndef TRANSFORM
#define TRANSFORM

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <assert.h>

#include <Eigen/Dense>
#include "utils.h"

// Externally public functions
void transformMatrix (Eigen::MatrixXd*, std::vector<Eigen::MatrixXd>*);
int generateTransform (std::string, Eigen::MatrixXd*);

#endif // ifndef TRANSFORM
