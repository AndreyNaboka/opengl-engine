#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();

  void Bind() const;
  void Unbind() const;

  void SetUniformMat4(const std::string &name, const glm::mat4 &matrix) const;
  void SetUniformVec3(const std::string &name, const glm::vec3 &vector) const;
  void SetUniformInt(const std::string &name, int value) const;

  unsigned int GetID() const { return _ID; }
  int GetUniformLocation(const std::string &name) const;

private:
  unsigned int _ID;
  mutable std::unordered_map<std::string, int> _cache;

  std::string ReadFile(const std::string &path) const;
  unsigned int Compile(unsigned int type, const std::string &source) const;
};
