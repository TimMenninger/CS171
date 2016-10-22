/******************************************************************************

 perspective.h

 Contains public functions from perspective.cpp.

 Author: Tim Menninger

******************************************************************************/
#ifndef PERSPECTIVE
#define PERSPECTIVE

#include <map>
#include <stdint.h>

#include "transform.h"
#include "objParser.h"
#include "parseScene.h"

void worldToCartNDC (camera, std::vector<shape3D>, std::vector<shape3D>*);
void worldToCamera (camera, std::vector<shape3D>, std::vector<shape3D>*);
void worldToCameraMatrix (camera, Eigen::MatrixXd*);
void perspectiveProjectionMatrix (camera, Eigen::MatrixXd*);

#endif // ifndef PERSPECTIVE
