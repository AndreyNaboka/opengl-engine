#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

class light
{
public:
   light(const glm::vec3 &pos, const std::string &name);
   void render();

private:
   std::string _name;
   glm::vec3 _pos;
};
