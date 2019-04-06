#include "Constants.h"
#include "States.h"
#include "Tree.h"
#include "Utils.h"
#include "ShaderProgram.h"
#include <iostream>
#include <math.h>
#include <glm/gtx/string_cast.hpp>

States::States(std::string full_path, std::string tree_hash) {
  trees.push_back(Tree(full_path, tree_hash));
}

void generateCircle(std::vector<Point>* v) {
  size_t i = 0;
  GLfloat step = 2 * M_PI / CIRCLE_COUNT_DIVISIONS;
  for (float degree = 0; i < CIRCLE_COUNT_DIVISIONS; i++, degree += step) {
    v->push_back(Point{cos(degree) * CIRCLE_RADIUS, sin(degree) * CIRCLE_RADIUS, 204.0/255, 0.0f, 204.0/255, 1.0f});
    v->push_back(Point{0, 0, 204.0/255, 0.0f, 204.0/255, 1.0f});
  }
  v->push_back(Point{CIRCLE_RADIUS, 0, 204.0/255, 0.0f, 204.0/255, 1.0f});
}

void States::initGraphic() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &VBO2);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);


  std::vector<Point> buffer;
  generateCircle(&buffer);
  glBufferData(GL_ARRAY_BUFFER, buffer.size()*sizeof(Point), &buffer[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void States::render() {
  renderCurrent();

  GLint modelLoc = glGetUniformLocation(shaderProgram->shaderProgramId, "model");
  GLint scaleLoc = glGetUniformLocation(shaderProgram->shaderProgramId, "scale");

  glm::mat4 scaleMatrix = glm::mat4(1.0f);
  scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, scale));
  glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(scaleMatrix));

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  for (size_t i = 0; i < leafTranslates.size(); i++) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f));
    model = glm::translate(model, leafTranslates[i]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, leafTranslates.size());
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO2);

  glBufferData(GL_ARRAY_BUFFER, bufferLines.size()*sizeof(Line), &bufferLines[0], GL_DYNAMIC_DRAW);
  
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f))));
  for (size_t i = 0; i < bufferLines.size(); i++) {
    glDrawArrays(GL_LINE_LOOP, i*2, 2);
  }
  glBindVertexArray(0);
}

void States::tick() {
  // for (size_t i = 0; i < trees[indexTree].countLeafs; i++) {
  //   leafTranslates[i] += glm::vec3(0.001f, 0.001f, 0.001f);
  // }
}

size_t countCirclesOnLevel(const size_t level) {
  return level > 0 ? (level == 1 ? 1 : 3 * level) : 0;
}

void States::drawCircle(float x, float y) {
  leafTranslates.push_back(glm::vec3(x, y, 0.0f));
}

void States::drawLine(float x1, float y1, float x2, float y2) {
  bufferLines.push_back(Line{
    Point{x1, y1, 204.0/255, 0.0f, 204.0/255, 1.0f},
    Point{x2, y2, 204.0/255, 0.0f, 204.0/255, 1.0f}
  });
}

bool verticeIsEmpty(Vertice* v) {
  return v->curves.size() == 0 && v->leafs.size() == 0;
}

void States::drawLeafs(GLfloat x, GLfloat y, Vertice *v) {
  size_t leafsLeft = v->leafs.size();
  
  if (leafsLeft == 2) {
    drawCircle(x, y + CIRCLE_RADIUS);
    drawCircle(x, y - GAP_BETWEEN_CIRCLES - CIRCLE_RADIUS);
    return;
  }

  if (leafsLeft > 0) {
    leafsLeft--;
    drawCircle(x, y);
  }

  size_t level = 1;
  while (leafsLeft > 0) {
    level++;
    size_t countCirclesOnLev = countCirclesOnLevel(level);
    size_t countCircles = min(leafsLeft, countCirclesOnLev);
    leafsLeft -= countCircles;

    GLfloat degrees = M_PI/2;
    if (countCircles == countCirclesOnLev && level % 2 == 0) {
      // на четных уровнях листья немного смещены относительно 90 градусов
      degrees += (CIRCLE_RADIUS + GAP_BETWEEN_CIRCLES/2)/((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES));
    }

    for (size_t i = 0; i < countCircles; i++, degrees += 2 * M_PI / countCircles) {
      drawCircle(x + cos(degrees) * ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)),
                y + sin(degrees) * ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)));
    }
  }
}

void States::renderCurrent() {
  indexTree = 0;
  leafTranslates.clear();
  bufferLines.clear();
  drawTree();
}

float calculateRadius(size_t leafsCount) {
  if (leafsCount == 0) {
    return CIRCLE_RADIUS;
  }

  size_t level = 1;
  while (leafsCount > 0) {
    level++;
    size_t countCirclesOnLev = countCirclesOnLevel(level);
    size_t countCircles = min(leafsCount, countCirclesOnLev);
    leafsCount -= countCircles;
  }

  return ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)) + CIRCLE_RADIUS;
}

float calculateMaxColumnRadius(Vertice* v) {
  float maxColumnRadius = calculateRadius(v->leafs.size());
  while (v->curves.size() > 0) {
    v = &v->curves[0].vertice;
    maxColumnRadius = max(maxColumnRadius, calculateRadius(v->leafs.size()));
  }

  return maxColumnRadius;
}

void States::drawVerticeAndLine(float x, float y, float childX, float childY, Vertice* v, std::string direction) {
  if (verticeIsEmpty(v)) {
    return;
  }

  drawLine(x, y, childX, childY);
  drawVertice(childX, childY, v, direction);
}

void States::drawVertice(float x, float y, Vertice* v, std::string direction) {
  float radius = calculateRadius(v->leafs.size());
  float curvesSize = v->curves.size();

  if (curvesSize > 0) {
    drawVerticeAndLine(x, y, x, y - radius - GAP_BETWEEN_BRANCHES*10 - calculateRadius(v->curves[0].vertice.leafs.size()), &v->curves[0].vertice, direction);

    for (size_t i = 1; i < curvesSize; i++) {
      Vertice* currentV = &v->curves[i].vertice;

      float childRadius = calculateMaxColumnRadius(currentV);
      float childY = y - radius - GAP_BETWEEN_BRANCHES*10 - childRadius;

      if (direction == "center") {
        if (i % 2 == 1) {
          drawVerticeAndLine(x, y, lBorder - GAP_BETWEEN_BRANCHES - childRadius, childY, currentV, "left");
        } else {
          drawVerticeAndLine(x, y, rBorder + GAP_BETWEEN_BRANCHES + childRadius, childY, currentV, "right");
        }
      } else if (direction == "left") {
        drawVerticeAndLine(x, y, lBorder - GAP_BETWEEN_BRANCHES - childRadius, childY, currentV, direction);  
      } else if (direction == "right") {
        drawVerticeAndLine(x, y, rBorder + GAP_BETWEEN_BRANCHES + childRadius, childY, currentV, direction);  
      }
    }
  }

  drawLeafs(x, y, v);
  float possibleLBorder = x - radius;
  float possibleRBorder = x + radius;

  if (possibleLBorder < lBorder) {
    lBorder = possibleLBorder;
  }

  if (possibleRBorder > rBorder) {
    rBorder = possibleRBorder;
  }
}

void States::drawTree() {
  lBorder = rBorder = 0.0f;
  drawVertice(0, 1 - GAP_BETWEEN_BRANCHES - calculateRadius(trees[indexTree].root.leafs.size()), &trees[indexTree].root, "center");
}
