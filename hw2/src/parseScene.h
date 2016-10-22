/******************************************************************************

 parseScene.h

 Contains public function declarations from parseScene.cpp.

 Author: Tim Menninger

******************************************************************************/
#ifndef PARSESCENE
#define PARSESCENE

#include <map>

#include <Eigen/Dense>

#include "objParser.h"
#include "transform.h"
#include "camera.h"
#include "light.h"

#define OBJ_DIR "data/"

// Externally public functions
int parseScene (char*, camera*, std::vector<light>*, std::vector<std::string>*, std::map<std::string, shape3D*>*, std::vector<shape3D>*);

#endif // ifndef PARSESCENE
