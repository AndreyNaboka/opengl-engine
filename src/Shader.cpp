#include "Shader.h"

Shader::Shader(const std::string &vertCode, const std::string &fragCode) {
  const char *v = vertCode.c_str();
  const char *f = fragCode.c_str();

  unsigned int vertexProgram = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexProgram, 1, &v, NULL);
  glCompileShader(vertexProgram);
  CheckShaderCompile(vertexProgram, "vertex");

  unsigned int fragmentProgram = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentProgram, 1, &f, NULL);
  glCompileShader(fragmentProgram);
  CheckShaderCompile(fragmentProgram, "fragment");

  _ID = glCreateProgram();
  glAttachShader(_ID, vertexProgram);
  glAttachShader(_ID, fragmentProgram);
  glLinkProgram(_ID);
  CheckShaderCompile(_ID, "program");

  glDeleteShader(vertexProgram);
  glDeleteShader(fragmentProgram);
}

void Shader::CheckShaderCompile(const unsigned int shader,
                                const std::string &type) {
  int success = 0;
  char infoLog[1024];
  if (type != "program") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      log("Shader " + std::to_string(shader) + ", compile error: " + infoLog);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      log("Shader " + std::to_string(shader) + ", link error: " + infoLog);
    }
  }
}
