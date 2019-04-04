#include <GL/glew.h>
#include <math.h>

const GLfloat CIRCLE_RADIUS = 0.02;
const GLfloat CIRCLE_D = CIRCLE_RADIUS*2;
const GLfloat GAP_BETWEEN_CIRCLES = 0.005;
const GLfloat CIRCLE_DIVISION = M_PI*(CIRCLE_RADIUS + GAP_BETWEEN_CIRCLES)/3;

const int CIRCLE_COUNT_DIVISIONS = 50;

const GLfloat LINE_WIDTH = 0.05;