#ifndef COMMON_HPP
#define COMMON_HPP

// This includes more headers than you will probably need.
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <utility> // pair
#include <queue>
#include <vector>
#include <array>
#include <cmath>
#include <cfloat>
#include <limits>
#include <fstream>
#include <algorithm> //swap
#include <cassert>
#include <unordered_map>
#include <unordered_set>

#include <thread>
#include <mutex>

#ifdef __APPLE__
    #include <OpenGL/glext.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>
#else
    #include <GL/glew.h>
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glext.h>
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util.hpp"
#include "util_linear_algebra.hpp"
#include "timer.hpp"
#include "hasher.hpp"
#include "debug.hpp"

#include "constants.hpp"

#endif
