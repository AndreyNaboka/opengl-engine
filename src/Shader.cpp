#include "Shader.h"
#include "Logger.h"
#include "PathUtils.h"
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  auto vPath = Path::ResolveAssetPath(vertexPath);
  auto fPath = Path::ResolveAssetPath(fragmentPath);

  unsigned int vs = Compile(GL_VERTEX_SHADER, ReadFile(vPath.string()));
  unsigned int fs = Compile(GL_FRAGMENT_SHADER, ReadFile(fPath.string()));

  _ID = glCreateProgram();
  glAttachShader(_ID, vs);
  glAttachShader(_ID, fs);
  glLinkProgram(_ID);

  int success;
  char log[512];
  glGetProgramiv(_ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(_ID, 512, nullptr, log);
    LogInfo(std::string("[Shader] link error: ") + std::string(log));
  }

  glDeleteShader(vs);
  glDeleteShader(fs);
}

Shader::~Shader() { glDeleteProgram(_ID); }
void Shader::Bind() const { glUseProgram(_ID); }
void Shader::Unbind() const { glUseProgram(0); }

int Shader::GetUniformLocation(const std::string &name) const {
  if (_cache.find(name) == _cache.end()) {
    int loc = glGetUniformLocation(_ID, name.c_str());
    if (loc == -1)
      LogInfo("[Shader] uniform " + name + " not found");
    _cache[name] = loc;
  }
  return _cache[name];
}

void Shader::SetUniformMat4(const std::string &name,
                            const glm::mat4 &matrix) const {
  glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}
void Shader::SetUniformVec3(const std::string &name,
                            const glm::vec3 &vector) const {
  glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(vector));
}
void Shader::SetUniformInt(const std::string &name, int value) const {
  glUniform1i(GetUniformLocation(name), value);
}

std::string Shader::ReadFile(const std::string &path) const {
  LogInfo("Try to open shader from " + path);
  std::ifstream file(path);
  if (!file.is_open())
    LogInfo("[Shader] Failed to open shader: " + path);
  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

unsigned int Shader::Compile(unsigned int type,
                             const std::string &source) const {
  unsigned int shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  int success;
  char log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, log);
    LogInfo(std::string("[Shader] compile error: " + std::string(log)));
  }
  return shader;
}
