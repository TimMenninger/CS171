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
#include "geom.h"
#include "vertex.h"
#include "facet.h"
#include "camera.h"
#include "normal.h"
#include "light.h"
#include "shape3d.h"

void worldToCartNDC (camera, std::vector<shape3D>*, std::vector<shape3D>*);

#endif // ifndef PERSPECTIVE
