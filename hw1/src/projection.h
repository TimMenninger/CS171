#ifndef PERSPECTIVE
#define PERSPECTIVE

#include <map>
#include <stdint.h>

#include "transform.h"
#include "objParser.h"
#include "parseScene.h"

void worldToCartNDC (camera, std::map<std::string, std::vector<shape3D> >, std::vector<shape3D>*);

#endif // ifndef PERSPECTIVE
