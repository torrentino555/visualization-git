#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Leaf;
class Curve;
class Vertice;
struct Point;
struct Line;

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
  std::string full_path;
  std::vector<Line>* bufferLines;
  std::vector<glm::vec3>* leafTranslates;
  std::vector<std::string> commitTrees;
  std::vector<std::string>::iterator currentTree;
  float lBorder;
  float rBorder;

  void add_vertice(Vertice *vertice, std::string tree_hash);
  void modifyVertice(Vertice*, std::string);
  bool nextTree();
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

  Tree(std::string, std::string, std::vector<glm::vec3>*, std::vector<Line>*);
  void render();
  void applyCommit(std::string);
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
