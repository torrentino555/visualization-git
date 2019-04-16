#ifndef _SHADER_H_
#define _SHADER_H_
#pragma once
#include <string>
#include <iostream>
#include <GL/glew.h>

class Shader {
public:
  GLuint shaderId;

  Shader(std::string code, GLenum shaderType) {
    const GLchar* shaderCode = code.c_str();
    shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::" <<
        (shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
        << "::COMPILATION_FAILED\n" << infoLog << std::endl;
      std::cout << "TEXT:" << std::endl;
      std::cout << code << std::endl;
    }
  }
};

#endif
