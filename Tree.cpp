#include "Tree.h"

#include "States.h"
#include "Utils.h"
#include <iostream>
#include <set>

const int TYPE_INDEX = 1;
const int HASH_INDEX = 2;
const int NAME_INDEX = 3;

Tree::Tree(std::string full_path, std::string tree_hash) : full_path(full_path) {
  add_vertice(&root, tree_hash);
}

void Tree::add_vertice(Vertice *vertice, std::string tree_hash) {
  std::vector<std::string> tree_lines = splitStringByDelimiter(get_comiit_or_tree_text(full_path, tree_hash), "\n");

  for (size_t i = 0; i < tree_lines.size(); i++) {
    std::vector<std::string> tokens = splitStringByTwoDelimiters(tree_lines[i], " ", "\t");

    if (tokens[TYPE_INDEX].find("blob") == 0) {
      countLeafs++;
      vertice->leafs.push_back(Leaf(tokens[NAME_INDEX], tokens[HASH_INDEX]));
    } else if (tokens[TYPE_INDEX].find("tree") == 0) {
      countCurves++;
      vertice->curves.push_back(Curve(tokens[NAME_INDEX], tokens[HASH_INDEX]));
      add_vertice(&vertice->curves[vertice->curves.size() - 1].vertice, tokens[HASH_INDEX]);
    }
  }
}

void Tree::applyCommit(std::string tree_hash) {
  
}

void Tree::modifyVertice(Vertice* v, std::string hash) {
  std::vector<std::string> tree_lines = splitStringByDelimiter(get_comiit_or_tree_text(full_path, hash), "\n");
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