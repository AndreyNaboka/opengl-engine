#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class shader
{
public:
   shader(const std::string &vertex_path, const std::string &fragment_path);
   void use() const;

   void set_mat4(const std::string &name, const glm::mat4 &mat) const;
   void set_vec3(const std::string &name, const glm::vec3 &value) const;
   void set_float(const std::string &name, float value) const;
   void set_int(const std::string &name, int value) const;

private:
   unsigned int _id;
   mutable std::unordered_map<std::string, int> _uniform_cache;
   int get_uniform_location(const std::string &name) const;
};