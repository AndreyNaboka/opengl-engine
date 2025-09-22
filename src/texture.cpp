#include "texture.h"
#include "logger.h"

std::shared_ptr<texture> texture::create(const std::string &name, const std::string &path)
{
   if (name.empty()) {
      logger::error("Try to create texture with empty name");
      return nullptr;
   }

   if (path.empty()) {
      logger::error("Try to create texture name: " + name + " with empty path to image");
      return nullptr;
   }

   logger::info("Create texture " + name + ", from " + path);

   return std::shared_ptr<texture>(new texture(name, path));
}

bool texture::load()
{
   return false;
}

void texture::bind()
{
   if (!_is_loaded) {
      logger::error("Try to bind unloaded texture with name " + _name);
      return;
   }
}

texture::texture(const std::string &name, const std::string &path)
{
   _name = name;
   _path = path;
}

texture::~texture()
{
   logger::info("release texture " + _name);
}