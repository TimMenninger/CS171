#ifndef TRANSFORMOBJS
#define TRANSFORMOBJS

#include <map>

#include <Eigen/Dense>
#include "../Part1/objParser.h"
#include "../Part2/transform.h"

// Externally public functions
void cleanupShapes (std::map<std::string, shape3D*>*, std::map<std::string, std::vector<shape3D> >*);
void printShapes (std::vector<std::string>*, std::map<std::string, shape3D*>*, std::map<std::string, std::vector<shape3D> >*);
int parseShapeTransforms (char*, std::vector<std::string>*, std::map<std::string, shape3D*>*, std::map<std::string, std::vector<shape3D> >*);

#endif // ifndef TRANSFORMOBJS
