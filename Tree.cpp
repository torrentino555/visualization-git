#include "Tree.h"
#include "Constants.h"
#include "States.h"
#include "Utils.h"
#include <iostream>
#include <set>
#include <stack>
#include <glm/gtx/string_cast.hpp>

const int TYPE_INDEX = 1;
const int HASH_INDEX = 2;
const int NAME_INDEX = 3;

Tree::Tree(std::string full_path, std::string last_commit_hash, std::vector<glm::vec3>* leafsTranslates, std::vector<Line>* bufferLines) 
: full_path(full_path), bufferLines(bufferLines), leafTranslates(leafsTranslates) {
  std::stack<std::string> commitsStack;
  std::set<std::string> commitsSet;
  commitsStack.push(last_commit_hash);
  commitsSet.insert(last_commit_hash);

  while (!commitsStack.empty()) {
    std::string commit_text = get_commit_or_tree_text(full_path, commitsStack.top());
    std::vector<std::string> lines = splitStringByDelimiter(commit_text, "\n");
    commitsStack.pop();

    for (auto line : lines) {
      if (line.find("tree") == 0) {
        commitTrees.push_back(line.substr(5));
      } else if (line.find("parent") == 0) {
        std::string parentCommit = line.substr(7);
        if (commitsSet.find(parentCommit) == commitsSet.end()) {
          commitsSet.insert(parentCommit);
          commitsStack.push(parentCommit);
        }
      }
    }
  }

  currentTree = --commitTrees.end();

  add_vertice(&root, *currentTree);
}

void Tree::render() {
  leafTranslates->clear();
  bufferLines->clear();
  drawTree();
}

bool Tree::nextTree() {
  if (currentTree != commitTrees.begin()) {
    currentTree++;
    return true;
  }

  return false;
}

void Tree::add_vertice(Vertice *vertice, std::string last_commit_hash) {
  std::vector<std::string> tree_lines = splitStringByDelimiter(get_commit_or_tree_text(full_path, last_commit_hash), "\n");

  for (size_t i = 0; i < tree_lines.size(); i++) {
    std::vector<std::string> tokens = splitStringByTwoDelimiters(tree_lines[i], " ", "\t");

    if (tokens[TYPE_INDEX].find("blob") == 0) {
      vertice->leafs.push_back(Leaf(tokens[NAME_INDEX], tokens[HASH_INDEX]));
    } else if (tokens[TYPE_INDEX].find("tree") == 0) {
      vertice->curves.push_back(Curve(tokens[NAME_INDEX], tokens[HASH_INDEX]));
      add_vertice(&vertice->curves[vertice->curves.size() - 1].vertice, tokens[HASH_INDEX]);
    }
  }
}

void Tree::applyCommit(std::string last_commit_hash) {
  
}

void Tree::modifyVertice(Vertice* v, std::string hash) {
  std::vector<std::string> tree_lines = splitStringByDelimiter(get_commit_or_tree_text(full_path, hash), "\n");
  std::set<Leaf> leafsToDelete(v->leafs.begin(), v->leafs.end());
  std::vector<Leaf> leaftToAdd;
  std::set<Curve> curvesToDelete(v->curves.begin(), v->curves.end());
  std::vector<Curve> curvesToAdd;

  for (size_t i = 0; i < tree_lines.size(); i++) {
    std::vector<std::string> tokens = splitStringByTwoDelimiters(tree_lines[i], " ", "\t");

    if (tokens[TYPE_INDEX].find("blob") == 0) {
      Leaf l(tokens[NAME_INDEX], tokens[HASH_INDEX]);
      auto it = leafsToDelete.find(l);
      if (it == leafsToDelete.end()) {
        leaftToAdd.push_back(l);
      } else {
        leafsToDelete.erase(it);
      }
    } else if (tokens[TYPE_INDEX].find("tree") == 0) {
      Curve c(tokens[NAME_INDEX], tokens[HASH_INDEX]);
      auto it = curvesToDelete.find(c);
      if (it == curvesToDelete.end()) {
        curvesToAdd.push_back(c);
        modifyVertice(&c.vertice, tokens[HASH_INDEX]);    
      } else {
        curvesToDelete.erase(it);
      }
    }
  }

  for (auto it = v->leafs.begin(); it != v->leafs.end();) {
    if (leafsToDelete.find(*it) != leafsToDelete.end()) {
      it = v->leafs.erase(it);
    } else {
      it++;
    }
  }

  v->leafs.insert(v->leafs.begin(), leaftToAdd.begin(), leaftToAdd.end());

  for (auto it = v->curves.begin(); it != v->curves.end();) {
    if (curvesToDelete.find(*it) != curvesToDelete.end()) {
      it = v->curves.erase(it);
    } else {
      it++;
    }
  }

  v->curves.insert(v->curves.begin(), curvesToAdd.begin(), curvesToAdd.end());
}


//////


size_t Tree::countCirclesOnLevel(const size_t level) {
  return level > 0 ? (level == 1 ? 1 : 3 * level) : 0;
}

void Tree::drawCircle(float x, float y) {
  leafTranslates->push_back(glm::vec3(x, y, 0.0f));
}

void Tree::drawLine(float x1, float y1, float x2, float y2) {
  bufferLines->push_back(Line{
    Point{x1, y1, 204.0/255, 0.0f, 204.0/255, 1.0f},
    Point{x2, y2, 204.0/255, 0.0f, 204.0/255, 1.0f}
  });
}

bool Tree::verticeIsEmpty(Vertice* v) {
  return v->curves.size() == 0 && v->leafs.size() == 0;
}

void Tree::drawLeafs(GLfloat x, GLfloat y, Vertice *v) {
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

float Tree::calculateRadius(size_t leafsCount) {
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

float Tree::calculateMaxColumnRadius(Vertice* v) {
  float maxColumnRadius = calculateRadius(v->leafs.size());
  while (v->curves.size() > 0) {
    v = &v->curves[0].vertice;
    maxColumnRadius = max(maxColumnRadius, calculateRadius(v->leafs.size()));
  }

  return maxColumnRadius;
}

void Tree::drawVerticeAndLine(float x, float y, float childX, float childY, Vertice* v, std::string direction) {
  if (verticeIsEmpty(v)) {
    return;
  }

  drawLine(x, y, childX, childY);
  drawVertice(childX, childY, v, direction);
}

void Tree::drawVertice(float x, float y, Vertice* v, std::string direction) {
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

void Tree::drawTree() {
  lBorder = rBorder = 0.0f;
  drawVertice(0, 1 - GAP_BETWEEN_BRANCHES - calculateRadius(root.leafs.size()), &root, "center");
}