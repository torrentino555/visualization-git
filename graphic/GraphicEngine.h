#ifndef _GRAPHIC_ENGINE_H_
#define _GRAPHIC_ENGINE_H_
#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <chrono>
#include "ShaderProgram.h"
#include "States.h"

class GraphicEngine {
private:
  const int WINDOW_WIDTH = 1080;
  const int WINDOW_HEIGHT = 1080;
  const std::string PROGRAM_NAME = "Визуализация логов Git";
  const std::string VERTEX_SHADER_PATH = "resources/vertex-shader";
  const std::string FRAGMENT_SHADER_PATH = "resources/fragment-shader";

  GLFWwindow *window = nullptr;
  ShaderProgram *colorProgram = nullptr;
  ShaderProgram *lastProgram = nullptr;
  States *commitTrees = nullptr;

  std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
  
  void handle_input();
  static void initGLFW();
  int initWindow();
  void render();

public:
  GraphicEngine(States *states) : commitTrees(states), lastUpdate(std::chrono::steady_clock::now()) {};
  int start();
};

#endif