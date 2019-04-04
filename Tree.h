#pragma once
#include <vector>
#include <string>

class Leaf;
class Curve;
class Vertice;

// вершина
class Vertice {
public:
  std::vector<Leaf> leafs;
  std::vector<Curve> curves;
};

class Tree {
private:
  std::string full_path;

  void add_vertice(Vertice *vertice, std::string tree_hash);
public:
  size_t countLeafs = 0;
  size_t countCurves = 0;

  Vertice root;

  Tree(std::string full_path, std::string tree_hash);
};

// лист
class Leaf {
private:
  std::string commit_hash;
public:
  std::string label;

  Leaf(std::string label, std::string commit_hash) : commit_hash(commit_hash), label(label) {};
};

// дуга
class Curve {
private:
  std::string commit_hash;
public:
  Vertice vertice;
  std::string label;

  Curve(std::string label, std::string commit_hash) : commit_hash(commit_hash), label(label) {};
};
