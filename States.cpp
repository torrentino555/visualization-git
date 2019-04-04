#include "Constants.h"
#include "States.h"
#include "Tree.h"
#include "Utils.h"
#include <iostream>
#include <math.h>

size_t countCirclesOnLevel(const size_t level) {
  return level > 0 ? (level == 1 ? 1 : 3 * level) : 0;
}

void States::initGraphic() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  renderCurrent();

  glBufferData(GL_ARRAY_BUFFER, currentLeafsCoordsSize*sizeof(GLfloat), leafsCoords,
               GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void States::addCircle(float x, float y) {
  size_t j = 0;
  GLfloat step = 2 * M_PI / CIRCLE_COUNT_DIVISIONS;
  for (float i = 0; i < 2 * M_PI; i += step, j++) {
    if (j == CIRCLE_COUNT_DIVISIONS) {
      break;
    }

    leafsCoords[indexLeafsCoords++] = x + cos(i) * CIRCLE_RADIUS;
    leafsCoords[indexLeafsCoords++] = y + sin(i) * CIRCLE_RADIUS;
    leafsCoords[indexLeafsCoords++] = x + cos(i + step) * CIRCLE_RADIUS;
    leafsCoords[indexLeafsCoords++] = y + sin(i + step) * CIRCLE_RADIUS;
    leafsCoords[indexLeafsCoords++] = x;
    leafsCoords[indexLeafsCoords++] = y;
  }
}

// TODO: переписать
float getBigCircleRadius(int countLeafs) {
  int level = 1;

  if (countLeafs == 0) {
    return 0;
  }

  while (countLeafs > 0) {
    countLeafs -= countCirclesOnLevel(level++);
  }
  level--;

  return level * (CIRCLE_D + GAP_BETWEEN_CIRCLES) + CIRCLE_D*3 +
         GAP_BETWEEN_CIRCLES;
}

void States::drawLeafs(GLfloat x, GLfloat y, Vertice *v) {
  size_t leafsLeft = v->leafs.size();
  std::cout << leafsLeft << " шариков" << std::endl;
  if (leafsLeft > 0) {
    leafsLeft--;
    addCircle(x, y);
  }

  size_t level = 1;
  while (leafsLeft > 0) {
    level++;
    size_t countCirclesOnLev = countCirclesOnLevel(level);
    size_t countCircles =
        countCirclesOnLev < leafsLeft ? countCirclesOnLev : leafsLeft;
    leafsLeft -= countCircles;

    GLfloat degrees = M_PI/2;
    if (countCircles == countCirclesOnLev && level % 2 == 0) {
      // на четных уровнях листья немного смещены относительно 90 градусов
      degrees += (CIRCLE_RADIUS + GAP_BETWEEN_CIRCLES/2)/((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES));
    }

    for (size_t i = 0; i < countCircles; i++, degrees += 2 * M_PI / countCircles) {
      addCircle(x + cos(degrees) * ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)),
                y + sin(degrees) * ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)));
    }
  }
}

States::States(std::string full_path, std::string tree_hash) {
  trees.push_back(Tree(full_path, tree_hash));
}

void States::renderCurrent() {
  Tree *currentTree = &trees[indexTree];
  currentLeafsCoordsSize =
      currentTree->countLeafs * 2 * (CIRCLE_COUNT_DIVISIONS) * 3
      + 2*2*currentTree->countCurves;

  if (leafsCoords) {
    delete[] leafsCoords;
  }
  leafsCoords = new GLfloat[currentLeafsCoordsSize];
  indexLeafsCoords = 0;
  indexCurvesCoords = currentTree->countLeafs * 2 * (CIRCLE_COUNT_DIVISIONS) * 3;

  drawCurves(0, 0, &currentTree->root, 0, 2);
}

void States::drawCurves(GLfloat x, GLfloat y, Vertice* v, int k, int w) {
  drawLeafs(x, y, v);
  size_t countChildrens = v->curves.size();
  GLfloat degrees = -k*(M_PI/4) + (w == 2 ? 0 : w == 0 ? M_PI/4 : -M_PI/4), rootCircleRadius = getBigCircleRadius(v->leafs.size());
  GLfloat rad = rootCircleRadius + GAP_BETWEEN_CIRCLES;
  GLfloat limit = w == 2 ? 2*M_PI : w == 0 ? 3*M_PI/4 : M_PI/4;
  for (size_t i = 0; i < countChildrens; i++, degrees += limit/countChildrens) {
    GLfloat r = rad + getBigCircleRadius(v->curves[i].vertice.leafs.size());
    std::cout << x << " " << y << " " << x + cos(degrees)*r << " " << y + sin(degrees)*r << std::endl;
    leafsCoords[indexCurvesCoords++] = x;
    leafsCoords[indexCurvesCoords++] = y;
    leafsCoords[indexCurvesCoords++] = x + cos(degrees)*r;
    leafsCoords[indexCurvesCoords++] = y + sin(degrees)*r;
    drawCurves(x + cos(degrees)*r, y + sin(degrees)*r, &v->curves[i].vertice, k + 1,
      degrees > M_PI/4 && degrees <= 3*M_PI/4 ? 0 : 1 );
  }
}

void States::render() {
  glBindVertexArray(VAO);
  for (size_t i = 0; i < trees[indexTree].countLeafs; i++) {
    glDrawArrays(GL_TRIANGLE_STRIP, i*CIRCLE_COUNT_DIVISIONS*3, CIRCLE_COUNT_DIVISIONS*3);
  }
  // glLineWidth(1);
  for (size_t i = 0; i < trees[indexTree].countCurves; i++) {
    glDrawArrays(GL_LINE_LOOP, trees[indexTree].countLeafs * (CIRCLE_COUNT_DIVISIONS) * 3 + i*2, 2);
  }
  glBindVertexArray(0);
}

void States::tick() {
  // TODO: index++;
}





//
//
//
//
//
//
//
//
//
//
// const float GAP = 0.005;
//
//
// float calculateRadius(size_t leafsCount) {
//   // считает радиус для leafsCount листьев
//   return leafsCount;
// }
//
// float checkVertice(Vertice* v) {
//   v->radius = calculateRadius(v->leafs.size());
//
//   if (v->curves.size() == 0) {
//     v->childRadius = v->radius;
//     return v->radius;
//   }
//
//   float radius = GAP*(v->curves.size() - 1);
//   for (size_t i = 0; i < v->curves.size(); i++) {
//     radius += checkVertice(&v->curves[i].vertice);
//   }
//
//   v->childRadius = radius;
//   return radius;
// }
//
// void setScale(float scale) {
//   // set scale
// }
//
// void drawVertice(float x, float y, Vertice* v) {
//   drawLeafs(x, y, v);
//   for (size_t i = 0; i < v->curves.size(); i++) {
//     drawVertice(x, y + v->radius + GAP + v->curves[i].vertice.radius, &v->curves[i].vertice);
//   }
// }
//
// void States::drawTree() {
//   float radius = checkVertice(&trees[indexTree].root);
//   drawVertice(0, 1 - GAP - trees[indexTree].root.radius, &trees[indexTree].root);
// }
