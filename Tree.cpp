#include "Tree.h"

#include "States.h"
#include "Utils.h"
#include <iostream>

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
    // std::cout << tokens[TYPE_INDEX] << " " << tokens[NAME_INDEX] << std::endl;

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
