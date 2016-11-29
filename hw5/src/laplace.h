/******************************************************************************

 laplace.h

 Contains function headers, macros and includes for laplace.cpp

 Author: Tim Menninger

******************************************************************************/

#ifndef LAPLACE
#define LAPLACE

#include "structs.h"
#include "halfedge.h"
#include "objParser.h"

#include <vector>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#define OP_NONZEROS 7 // Number of assumed nonzeroes in Sparse Matrix
#define MIN_AREA 1e-10 // Minimum area before we assume zero

void smoothObjects(std::vector<Object> *objs, double h);

#endif // ifndef LAPLACE
