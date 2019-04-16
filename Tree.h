#include <utility>

#include <utility>

#ifndef _TREE_H_
#define _TREE_H_
#pragma once
#include <vector>
#include <string>
#include <map>
#include <GL/glew.h>
#include "WorkWithGit.h"
#include "Animations.h"
#include "Action.h"
#include "PointAndLine.h"

class Leaf;
class Curve;
class Vertice;
class WorkWithGit;
class Animations;

// вершина
class Vertice {
public:
  bool isEmpty = false;
  size_t new_leafs_count = 0;
  float oldX = 0, oldY = 0, newX = 0, newY = 0;
  std::map<std::string, Curve*> curvesMap;
  std::vector<Curve*> curves;
  std::vector<Leaf*> leafs;
  std::vector<Leaf*> leafs_to_add;
  std::vector<std::string> leafs_to_delete;
  std::vector<std::string> curves_to_delete;
  std::map<std::string, Curve*> curves_to_add;
};

class Tree {
private:
  WorkWithGit* git;
  Animations* animations;
  std::vector<Line>* bufferLines;
  std::vector<Point>* bufferCircles;
  float lBorder;
  float rBorder;

  void deleteStage(Vertice* v);
  void checkMax(float, float);

  bool curveIsEmpty(Curve* curve, float, float, float, float);
  void postDrawAndLine(float x, float y, float childX, float childY, Curve* curve, std::string direction);
  void postDraw(Vertice* v, float x, float y, const std::string& direction);
  void deletePath(Vertice*, std::string);
  void modifyPath(Vertice*, std::string);
  void preDraw(Vertice*, float, float, std::string);
  static void addPath(Vertice* v, std::string path);
  void moveLeafs(std::vector<Leaf*>* leafs, GLfloat deltaX, GLfloat deltaY);
  void redrawLeafs(std::vector<Leaf*>* newLeafs, GLfloat x, GLfloat y, GLfloat, GLfloat);
  void drawLeafs(std::vector<Leaf*>* newLeafs, GLfloat x, GLfloat y);
  void drawOrMoveCircle(Leaf* leaf, GLfloat x, GLfloat y, GLfloat, GLfloat);

  void drawCircle(Leaf* v, GLfloat x, GLfloat y, const std::string& color);
  void drawLine(Curve*, float x1, float y1, float x2, float y2, const std::string&);
  void predrawVerticeAndLine(float x, float y, float childX, float childY, Curve*, std::string direction);
  float calculateMaxColumnRadius(Vertice* v);
  float calculateRadius(size_t leafsCount);
  static size_t countCirclesOnLevel(const size_t level);
public:
  float xMax{};
  float xMin{};
  float yMax{};
  float yMin{};
  Vertice root;

  Tree(std::string, std::vector<Point>*, std::vector<Line>*);
  void render();
  // void applyCommit(std::string);
  void addFile(const std::string& path);
  void removeFile(const std::string& path);
  void modifyFile(const std::string& path);
};

// лист
class Leaf {
public:
  std::string label;
  size_t index = static_cast<size_t>(-1);
  bool newLeaf = true;

  Leaf(std::string label) : label(std::move(label)) {};
  bool operator< (const Leaf& e) const {
    return label.compare(e.label);
  };
};

// дуга
class Curve {
public:
  Vertice vertice;
  size_t index = static_cast<size_t>(-1);
  std::string label;

  Curve(){};
  Curve(std::string label) : label(std::move(label)) {};
  Curve(size_t index) : index(index) {};
  bool operator< (const Curve& e) const {
    return index < e.index;
  };
};

#endif