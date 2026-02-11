#pragma once

#include <string>
#include <memory>

#include "glad/glad.h"

class Texture
{
public:
   static std::shared_ptr<Texture> Create(const std::string &name, const std::string &path);
   void Load();
   void Bind();

private:
   Texture(const std::string &name, const std::string &path);

private:
   bool _inited = false;
   GLuint _id = 0;
   std::string _path;
   std::string _name;
};