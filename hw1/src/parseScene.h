#ifndef PARSESCENE
#define PARSESCENE

#include <map>

#include <Eigen/Dense>
#include "objParser.h"
#include "transform.h"
#include "camera.h"

#define OBJ_DIR "data/"

// Externally public functions
int parseScene (char*, camera*, std::vector<std::string>*, std::map<std::string, shape3D*>*, std::map<std::string, std::vector<shape3D> >*);

#endif // ifndef PARSESCENE
