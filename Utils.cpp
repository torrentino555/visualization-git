#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

std::string readFile(std::string path) {
  std::string text;
  std::ifstream file;
  file.exceptions(std::ifstream::badbit);
  try {
    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    text = stream.str();
    file.close();
  } catch (std::ifstream::failure e) {
    std::cout << "Error read file:" << path << std::endl;
  }
  
  return text;
}

float min(float x, float y) {
  return x > y ? y : x;
}

float max(float x, float y) {
  return x > y ? x : y;
}

size_t min(size_t x, size_t y) {
    return x > y ? y : x;
}

size_t max(size_t x, size_t y) {
    return x > y ? x : y;
}