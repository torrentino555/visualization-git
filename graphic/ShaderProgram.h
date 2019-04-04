#pragma once
#include "Shader.h"
#include <GL/glew.h>

class ShaderProgram {
public:
  GLuint shaderProgramId;

  ShaderProgram(std::string vertexShaderText, std::string fragmentShaderText) {
    shaderProgramId = glCreateProgram();

    Shader vertexSh(vertexShaderText, GL_VERTEX_SHADER);
    Shader fragmentSh(fragmentShaderText, GL_FRAGMENT_SHADER);

    glAttachShader(shaderProgramId, vertexSh.shaderId);
    glAttachShader(shaderProgramId, fragmentSh.shaderId);
    glLinkProgram(shaderProgramId);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexSh.shaderId);
    glDeleteShader(fragmentSh.shaderId);
  }

  void use() {
    glUseProgram(shaderProgramId);
  }
};
