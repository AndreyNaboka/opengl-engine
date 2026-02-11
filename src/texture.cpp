#include "Texture.h"
#include "Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::shared_ptr<Texture> Texture::Create(const std::string &name, const std::string &path)
{
   if (name.empty())
   {
      Logger::Error("Try to create texture with empty name");
      return nullptr;
   }

   if (path.empty())
   {
      Logger::Error("Try to create texture name: " + name + " with empty path to image");
      return nullptr;
   }

   Logger::Info("Create texture " + name + ", from " + path);

   return std::shared_ptr<Texture>(new Texture(name, path));
}

void Texture::Load()
{
   int width, height, channels;
   stbi_uc *imageData = stbi_load(_path.c_str(), &width, &height, &channels, 0);
   if (!imageData)
   {
      Logger::Error("Can't read image " + _name);
      return;
   }

   glGenTextures(1, &_id);
   glBindTexture(GL_TEXTURE_2D, _id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   // Set texture filtering parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   if (channels == 3)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
   else if (channels == 4)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
   glGenerateMipmap(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);

   stbi_image_free(imageData);
   _inited = true;
}

void Texture::Bind()
{
   if (!_inited)
   {
      Logger::Error("try to bind not inited texture - " + _name);
      return;
   }
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, _id);
}

Texture::Texture(const std::string &name, const std::string &path)
{
   _name = name;
   _path = path;
}
