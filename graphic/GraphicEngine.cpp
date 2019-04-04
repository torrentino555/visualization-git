#include "GraphicEngine.h"
#define GLEW_STATIC
#include "ShaderProgram.h"
#include "Utils.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// TODO: добавить resize

// true - отрисовывать линиями, false - заполнять цветом
bool polygonMode = false;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    polygonMode = !polygonMode;
    if (polygonMode) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
  }
}

void GraphicEngine::initGLFW() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //Выключение возможности изменения размера окна
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}

int GraphicEngine::initWindow() {
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PROGRAM_NAME.c_str(),
                            nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  return 0;
}

int GraphicEngine::start() {
  initGLFW();
  int returnVal = initWindow();
  if (returnVal == -1) {
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  colorProgram = new ShaderProgram(readFile(VERTEX_SHADER_PATH),
                                   readFile(FRAGMENT_SHADER_PATH));

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  commitTrees->initGraphic();

  glBindVertexArray(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    render();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

void GraphicEngine::render() {
  glClear(GL_COLOR_BUFFER_BIT);

  if (!lastProgram) {
    lastProgram = colorProgram;
    colorProgram->use();
  }

  commitTrees->render();
}
