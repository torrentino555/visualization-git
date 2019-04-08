#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "Tree.h"
#include "ShaderProgram.h"

struct Point {
  GLfloat x, y, r, g, b, a;
};

struct Line {
  Point first, second;
};

class States {
private:
  Tree* tree;
  GLuint VBO, VAO, VBO2;

  std::vector<Line> bufferLines;
  std::vector<Point> bufferCircles;
public:
  ShaderProgram *shaderProgram = nullptr;
  GLfloat scale = 1.0f;
  GLfloat offsetX = 0.0f, offsetY = 0.0f;

  States(std::string);

  void initGraphic();
  void render();
};
