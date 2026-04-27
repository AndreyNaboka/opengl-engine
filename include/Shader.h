#pragma once
#include "Logger.h"
#include <glad/gl.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <string>

class Shader {
public:
  static std::optional<Shader> Create(const std::string &v,
                                      const std::string &f) {
    if (v.empty() || f.empty()) {
      log("Shaders code can't be empty");
      return std::nullopt;
    }
    return Shader(v, f);
  }


  const unsigned int GetID() const { return _ID; }
  void Use() { glUseProgram(_ID); }
  void setMat4(const std::string &name, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(mat));
  }
  void setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(_ID, name.c_str()), value);
  }

private:
  Shader(const std::string &vertCode, const std::string &fragCode);
  void CheckShaderCompile(const unsigned int shader, const std::string &type);

private:
  unsigned int _ID = 0;
};
