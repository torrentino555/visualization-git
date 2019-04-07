#include "Constants.h"
#include "States.h"
#include "Tree.h"
#include "Utils.h"
#include "ShaderProgram.h"
#include <iostream>
#include <math.h>
#include <glm/gtx/string_cast.hpp>

States::States(std::string full_path, std::string lastCommitHash) {
  tree = new Tree(full_path, lastCommitHash, &leafTranslates, &bufferLines);
}

void generateCircle(std::vector<Point>* v) {
  size_t i = 0;
  GLfloat step = 2 * M_PI / CIRCLE_COUNT_DIVISIONS;
  for (float degree = 0; i < CIRCLE_COUNT_DIVISIONS; i++, degree += step) {
    v->push_back(Point{cos(degree) * CIRCLE_RADIUS, sin(degree) * CIRCLE_RADIUS, 204.0/255, 0.0f, 204.0/255, 1.0f});
    v->push_back(Point{0, 0, 204.0/255, 0.0f, 204.0/255, 1.0f});
  }
  v->push_back(Point{CIRCLE_RADIUS, 0, 204.0/255, 0.0f, 204.0/255, 1.0f});
}

void States::initGraphic() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &VBO2);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);


  std::vector<Point> buffer;
  generateCircle(&buffer);
  glBufferData(GL_ARRAY_BUFFER, buffer.size()*sizeof(Point), buffer.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void States::render() {
  tree->render();

  GLint modelLoc = glGetUniformLocation(shaderProgram->shaderProgramId, "model");
  GLint scaleLoc = glGetUniformLocation(shaderProgram->shaderProgramId, "scale");

  glm::mat4 scaleMatrix = glm::mat4(1.0f);
  scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, scale));
  glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(scaleMatrix));

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  for (auto translateLeaf : leafTranslates) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f));
    model = glm::translate(model, translateLeaf);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, CIRCLE_COUNT_DIVISIONS*2 + 1);
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO2);

  glBufferData(GL_ARRAY_BUFFER, bufferLines.size()*sizeof(Line), bufferLines.data(), GL_DYNAMIC_DRAW);
  
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f))));
  for (size_t i = 0; i < bufferLines.size(); i++) {
    glDrawArrays(GL_LINE_LOOP, i*2, 2);
  }
  glBindVertexArray(0);
}
