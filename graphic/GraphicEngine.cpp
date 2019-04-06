#include "GraphicEngine.h"
#define GLEW_STATIC
#include "ShaderProgram.h"
#include "Utils.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <map>
#include <queue>

// TODO: добавить resize

// true - отрисовывать линиями, false - заполнять цветом
bool polygonMode = false;

struct key_event {
    int key, code, action, modifiers;
    std::chrono::steady_clock::time_point time_of_event;
    key_event(int key, int code, int action, int modifiers, std::chrono::steady_clock::time_point time_of_event) 
      : key(key), code(code), action(action), modifiers(modifiers), time_of_event(time_of_event) {};
};

std::map<int, bool> keys;
std::queue<key_event> unhandled_keys;
void handle_key(GLFWwindow* window, int key, int code, int action, int modifiers) {
    unhandled_keys.emplace(key, code, action, modifiers, std::chrono::steady_clock::now());
}

void pressKeys(GLFWwindow *window, int key) {
  switch (key)
  {
    case GLFW_KEY_TAB:
      polygonMode = !polygonMode;
      if (polygonMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }  
      break;
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GL_TRUE);
      break;
  
    default:
      break;
  }
}

void GraphicEngine::handle_input() {
    auto now = std::chrono::steady_clock::now();
    float delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count()/1000.0f;
    lastUpdate = now;
    //Anything that should happen "when the users presses the key" should happen here
    while(!unhandled_keys.empty()) {
        key_event event = unhandled_keys.front();
        unhandled_keys.pop();
        pressKeys(window, event.key);
        bool pressed = event.action == GLFW_PRESS || event.action == GLFW_REPEAT;
        keys[event.key] = pressed;
    }
    //Anything that should happen "while the key is held down" should happen here.
    float speedScaleInSecond = 0.5;
    if(keys[GLFW_KEY_W]) commitTrees->offsetY -= delta_time;
    if(keys[GLFW_KEY_S]) commitTrees->offsetY += delta_time;
    if(keys[GLFW_KEY_A]) commitTrees->offsetX += delta_time;
    if(keys[GLFW_KEY_D]) commitTrees->offsetX -= delta_time;
    if(keys[GLFW_KEY_MINUS]) {
      float scaleAddValue = speedScaleInSecond*(delta_time);
      if (commitTrees->scale - scaleAddValue > 0) {
        commitTrees->scale -= scaleAddValue;
      }
    }
    if (keys[GLFW_KEY_EQUAL]) {
      commitTrees->scale += speedScaleInSecond*(delta_time);
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

  glfwSetKeyCallback(window, handle_key);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  colorProgram = new ShaderProgram(readFile(VERTEX_SHADER_PATH),
                                   readFile(FRAGMENT_SHADER_PATH));

  commitTrees->shaderProgram = colorProgram;

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  commitTrees->initGraphic();

  glBindVertexArray(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    handle_input();

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
  // commitTrees->tick();
}
