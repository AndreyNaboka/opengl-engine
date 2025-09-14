#pragma once

#include <string>
#include <memory>
#include "glad/glad.h"

class shader
{
public:
   static std::shared_ptr<shader> create(const std::string& name, const std::string& vertex_code, const std::string& fragment_code);
   GLuint get_program() const { return _program ;}
   void bind_shader();

private:
   shader(const std::string& name, const std::string& vertex_code, const std::string& fragment_code);

private:
   std::string _name;
   std::string _fragment_code;
   std::string _vertex_code;
   GLuint _program = 0;
};