#include "light.h"
#include "logger.h"

light::light(const glm::vec3 &pos, const std::string &name)
    : _pos(pos), _name(name)
{
}

void light::render()
{
}