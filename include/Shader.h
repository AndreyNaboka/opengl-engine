#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  void Bind() const;
  void Unbind() const;

  void SetUniformMat4(const std::string &name, const glm::mat4 &matrix) const;
  void SetUniformVec3(const std::string &name, const glm::vec3 &vector) const;
  void SetUniformInt(const std::string &name, int value) const;
  void SetUniformFloat(const std::string &name, float value) const;
  unsigned int GetID() const { return _ID; }
  bool IsValid() const { return _ID != 0; }
  int GetUniformLocation(const std::string &name) const;
  void BindUniformBlock(const std::string &name,
                        unsigned int bindingPoint) const;

private:
  unsigned int _ID;
  mutable std::unordered_map<std::string, int> _cache;

  bool ReadFile(const std::string &path, std::string &source) const;
  unsigned int Compile(unsigned int type, const std::string &source,
                       bool &success) const;
};
