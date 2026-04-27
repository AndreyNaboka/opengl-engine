#pragma once
#include "Logger.h"
#include <glad/gl.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <string>

class Shader {
public:
  unsigned int GetID() const { return _ID; }
  static std::optional<Shader> Create(const std::string &v,
                                      const std::string &f) {
    if (v.empty() || f.empty()) {
      log("Shaders code can't be empty");
      return std::nullopt;
    }
    return Shader(v, f);
  }

  void Use() { glUseProgram(_ID); }
  void setMat4(const std::string &name, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(mat));
  }
  void setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(_ID, name.c_str()), value);
  }

private:
  Shader(const std::string &vertCode, const std::string &fragCode) {
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
  };

  void CheckShaderCompile(const unsigned int shader, const std::string &type) {
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

private:
  unsigned int _ID = 0;
};
