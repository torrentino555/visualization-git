#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <GL/glew.h>
#include "Tree.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class States {
private:
  std::vector<Tree> trees;
  size_t indexTree = 0;
  size_t indexBufferLines;

  GLuint VBO, VAO, VBO2;

  GLfloat* bufferLines = nullptr;

  GLfloat* leafsCoords = nullptr;
  glm::vec3* leafTranslates = nullptr;
  size_t indexLeafsCoords;
  size_t indexCurvesCoords;
  size_t currentLeafsCoordsSize; // не уверен что нужна эта переменная

  float lBorder;
  float rBorder;

  void drawLeafs(GLfloat x, GLfloat y, Vertice* v);
  void drawCircle(float x, float y);
  void drawCurves(GLfloat x, GLfloat y, Vertice* v, int k, int);
  void drawLine(float x1, float y1, float x2, float y2);
  void drawTree();
  void drawVertice(float x, float y, Vertice* v, std::string);
  void drawVerticeAndLine(float x, float y, float childX, float childY, Vertice* v, std::string direction);
public:
  ShaderProgram *shaderProgram = nullptr;
  GLfloat scale = 1.0f;
  GLfloat offsetX = 0.0f, offsetY = 0.0f;

  States(std::string full_path, std::string tree_hash);

  void renderCurrent();
  void tick();
  void initGraphic();
  void render();
};
