#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader
{
public:
   Shader(const std::string &vertexPath, const std::string &fragmentPath);
   void Use() const;

   void SetMat4(const std::string &name, const glm::mat4 &mat) const;
   void SetVec3(const std::string &name, const glm::vec3 &value) const;
   void SetFloat(const std::string &name, float value) const;
   void SetInt(const std::string &name, int value) const;

private:
   unsigned int _id;
   mutable std::unordered_map<std::string, int> _uniformCache;
   int GetUniformLocation(const std::string &name) const;
};