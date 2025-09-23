#pragma once

#include <string>
#include <memory>

#include "glad/glad.h"

class texture 
{
public:
   static std::shared_ptr<texture> create(const std::string& name, const std::string& path);   
   void load();
   void bind();
      
private:
   texture(const std::string& name, const std::string& path);

private: 
   bool _inited = false;
   GLuint _id = 0;
   std::string _path;
   std::string _name;
};