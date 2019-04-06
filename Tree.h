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

  float radius = 0.0f;
  float childRadius = 0.0f;
};

class Tree {
private:
  std::string full_path;

  void add_vertice(Vertice *vertice, std::string tree_hash);
  void modifyVertice(Vertice*, std::string);
public:
  Vertice root;

  Tree(std::string full_path, std::string tree_hash);
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
