#ifndef _TREE_H_
#define _TREE_H_
#pragma once
#include <vector>
#include <string>
#include <GL/glew.h>
#include "WorkWithGit.h"

class Leaf;
class Curve;
class Vertice;
struct Point;
struct Line;
class WorkWithGit;

// вершина
class Vertice {
public:
  std::vector<Leaf> leafs;
  std::vector<Curve> curves;

  float radius = 0.0f;
  float childRadius = 0.0f;
};

class Tree {
private:
  WorkWithGit* git;
  std::vector<Line>* bufferLines;
  std::vector<Point>* bufferCircles;
  float lBorder;
  float rBorder;

  void addPath(Vertice*, std::string);

  void add_vertice(Vertice *vertice, git_tree*);
  void modifyVertice(Vertice*, std::string);
  void drawLeafs(GLfloat x, GLfloat y, Vertice* v);
  void drawCircle(float x, float y);
  void drawCurves(GLfloat x, GLfloat y, Vertice* v, int k, int);
  void drawLine(float x1, float y1, float x2, float y2);
  void drawTree();
  void drawVertice(float x, float y, Vertice* v, std::string);
  void drawVerticeAndLine(float x, float y, float childX, float childY, Vertice* v, std::string direction);
  float calculateMaxColumnRadius(Vertice* v);
  float calculateRadius(size_t leafsCount);
  bool verticeIsEmpty(Vertice* v);
  size_t countCirclesOnLevel(const size_t level);
public:
  Vertice root;

  Tree(std::string, std::vector<Point>*, std::vector<Line>*);
  void render();
  void applyCommit(std::string);
  void addFile(std::string path);
  void removeFile(std::string path);
  void modifyFile(std::string path);
};

// лист
class Leaf {
public:
  std::string commit_hash;
  std::string label;

  Leaf(std::string label, std::string commit_hash) : commit_hash(commit_hash), label(label) {};
  bool operator< (const Leaf& e) const {
    return commit_hash.compare(e.commit_hash);
  };
};

// дуга
class Curve {
private:
  std::string commit_hash;
public:
  Vertice vertice;
  std::string label;

  Curve(std::string label, std::string commit_hash) : commit_hash(commit_hash), label(label) {};
  bool operator< (const Curve& e) const {
    return commit_hash.compare(e.commit_hash);
  };
};

#endif