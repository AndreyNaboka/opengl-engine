#include "Shader.h"
#include "Utils/Logger.h"
#include "Utils/PathUtils.h"
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  auto vPath = Path::ResolveAssetPath(vertexPath);
  auto fPath = Path::ResolveAssetPath(fragmentPath);

  std::string vertexSource;
  std::string fragmentSource;
  if (!ReadFile(vPath.string(), vertexSource) ||
      !ReadFile(fPath.string(), fragmentSource)) {
    _ID = 0;
    return;
  }

  bool vertexOk = false;
  bool fragmentOk = false;
  unsigned int vs = Compile(GL_VERTEX_SHADER, vertexSource, vertexOk);
  unsigned int fs = Compile(GL_FRAGMENT_SHADER, fragmentSource, fragmentOk);
  if (!vertexOk || !fragmentOk) {
    if (vs)
      glDeleteShader(vs);
    if (fs)
      glDeleteShader(fs);
    _ID = 0;
    return;
  }

  _ID = glCreateProgram();
  glAttachShader(_ID, vs);
  glAttachShader(_ID, fs);
  glLinkProgram(_ID);

  int success;
  char log[512];
  glGetProgramiv(_ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(_ID, 512, nullptr, log);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteProgram(_ID);
    _ID = 0;
    LogInfo(std::string("[Shader] link error: ") + std::string(log));
    return;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  BindUniformBlock("Camera", 0);
  BindUniformBlock("Bones", 1);
}

Shader::~Shader() { glDeleteProgram(_ID); }
void Shader::Bind() const {
  if (_ID)
    glUseProgram(_ID);
}
void Shader::Unbind() const { glUseProgram(0); }

int Shader::GetUniformLocation(const std::string &name) const {
  if (!_ID)
    return -1;
  if (_cache.find(name) == _cache.end()) {
    int loc = glGetUniformLocation(_ID, name.c_str());
    _cache[name] = loc;
  }
  return _cache[name];
}

void Shader::BindUniformBlock(const std::string &name,
                              unsigned int bindingPoint) const {
  if (!_ID)
    return;

  const unsigned int index = glGetUniformBlockIndex(_ID, name.c_str());
  if (index == GL_INVALID_INDEX)
    return;
  glUniformBlockBinding(_ID, index, bindingPoint);
}

void Shader::SetUniformMat4(const std::string &name,
                            const glm::mat4 &matrix) const {
  const int location = GetUniformLocation(name);
  if (location == -1)
    return;
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::SetUniformVec3(const std::string &name,
                            const glm::vec3 &vector) const {
  const int location = GetUniformLocation(name);
  if (location == -1)
    return;
  glUniform3fv(location, 1, glm::value_ptr(vector));
}
void Shader::SetUniformInt(const std::string &name, int value) const {
  const int location = GetUniformLocation(name);
  if (location == -1)
    return;
  glUniform1i(location, value);
}

void Shader::SetUniformFloat(const std::string &name, float value) const {
  const int location = GetUniformLocation(name);
  if (location == -1)
    return;
  glUniform1f(location, value);
}

bool Shader::ReadFile(const std::string &path, std::string &source) const {
  LogInfo("[Shader] open shader " + path);
  std::ifstream file(path);
  if (!file.is_open()) {
    LogInfo("[Shader] Failed to open shader: " + path);
    source.clear();
    return false;
  }
  source.assign(std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>());
  return true;
}

unsigned int Shader::Compile(unsigned int type,
                             const std::string &source, bool &success) const {
  success = false;
  unsigned int shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  int compileOk;
  char log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileOk);
  if (!compileOk) {
    glGetShaderInfoLog(shader, 512, nullptr, log);
    LogInfo(std::string("[Shader] compile error: ") + std::string(log));
    return shader;
  }
  success = true;
  return shader;
}
