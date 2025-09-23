#include "texture.h"
#include "logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

void texture::load()
{
   int width, height, channels;
   stbi_uc* image_data = stbi_load(_path.c_str(), &width, &height, &channels, 0);
   if (!image_data) {
      logger::error("Can't read image " + _name);
      return;
   }

   glGenTextures(1, &_id);
   glBindTexture(GL_TEXTURE_2D, _id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   // Set texture filtering parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   if (channels == 3)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
   else if (channels == 4)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
   glGenerateMipmap(GL_TEXTURE_2D);

   stbi_image_free(image_data);
   _inited = true;
}

void texture::bind()
{
   if (!_inited) {
      logger::error("try to bind not inited texture - " + _name);
      return;
   }
   glBindTexture(GL_TEXTURE_2D, _id);
}

texture::texture(const std::string &name, const std::string &path)
{
   _name = name;
   _path = path;
}
