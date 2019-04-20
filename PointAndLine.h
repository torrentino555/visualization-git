#ifndef _POINT_AND_LINE_
#define _POINT_AND_LINE_
#include <GL/glew.h>
#include "RGBA.h"

struct Point {
  GLfloat x, y;
  RGBA color;
};

struct Line {
  Point first, second;
};

#endif