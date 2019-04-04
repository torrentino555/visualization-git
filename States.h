#pragma once
#include <string>
#include <GL/glew.h>
#include "Tree.h"

class States {
private:
  std::vector<Tree> trees;
  size_t indexTree = 0;

  GLuint VBO, VAO;

  GLfloat* leafsCoords = nullptr;
  size_t indexLeafsCoords;
  size_t indexCurvesCoords;
  size_t currentLeafsCoordsSize; // не уверен что нужна эта переменная

  void drawLeafs(GLfloat x, GLfloat y, Vertice* v);
  void addCircle(float x, float y);
  void drawCurves(GLfloat x, GLfloat y, Vertice* v, int k, int);
public:
  States(std::string full_path, std::string tree_hash);

  void renderCurrent();
  void tick();
  void initGraphic();
  void render();
};
