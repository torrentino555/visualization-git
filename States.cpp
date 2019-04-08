#include "Constants.h"
#include "States.h"
#include "Tree.h"
#include "Utils.h"
#include "ShaderProgram.h"
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

States::States(std::string full_path) {
  tree = new Tree(full_path, &bufferCircles, &bufferLines);
}

void States::initGraphic() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void States::render() {
  glBindVertexArray(VAO);

  tree->render();

  GLint modelLoc = glGetUniformLocation(shaderProgram->shaderProgramId, "model");
  GLint scaleLoc = glGetUniformLocation(shaderProgram->shaderProgramId, "scale");

  glm::mat4 scaleMatrix = glm::mat4(1.0f);
  scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, scale));
  glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(scaleMatrix));

  glBufferData(GL_ARRAY_BUFFER, bufferCircles.size()*sizeof(Line), bufferCircles.data(), GL_DYNAMIC_DRAW);

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f))));
  for (size_t i = 0; i < bufferCircles.size(); i++) {
    glDrawArrays(GL_TRIANGLE_STRIP, i*(CIRCLE_COUNT_DIVISIONS*2 + 1), CIRCLE_COUNT_DIVISIONS*2 + 1);
  }

  glBufferData(GL_ARRAY_BUFFER, bufferLines.size()*sizeof(Line), bufferLines.data(), GL_DYNAMIC_DRAW);

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f))));
  for (size_t i = 0; i < bufferLines.size(); i++) {
    glDrawArrays(GL_LINE_LOOP, i*2, 2);
  }
}
