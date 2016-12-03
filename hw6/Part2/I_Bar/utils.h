/******************************************************************************

 utils.h

 Contains public function declarations from utils.cpp.

 Author: Tim Menninger

******************************************************************************/
#ifndef UTILS
#define UTILS

#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>

#define PI 3.1415926

std::vector<std::string> getSpaceDelimitedWords (std::string line);
void rotationToQuaternion(float *rotation, float *quaternion);
void quaternionToRotation(float *quaternion, float *rotation);


#endif // ifndef UTILS
