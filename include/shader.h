#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "glad/glad.h"

class shader
{
public:
   static std::shared_ptr<shader> create(const std::string& name, const std::string& vertex_code, const std::string& fragment_code);
   void bind() { glUseProgram(_program); };
   GLint get_uniform_loc(const std::string& name);

private:
   shader(const std::string& name, const std::string& vertex_code, const std::string& fragment_code);

private:
   bool _is_inited = false;   
   std::unordered_map<std::string, GLint> _uniforms;
   std::string _name;
   std::string _fragment_code;
   std::string _vertex_code;
   GLuint _program = 0;
};