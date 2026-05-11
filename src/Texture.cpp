#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "Logger.h"
#include "Texture.h"
#include "stb_image.h"

Texture::Texture(const std::string &path) {
  if (path.empty())
   LogInfo("Can't create texture with empty path");

  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(path.c_str(), &_width, &_height, &_channels, 0);
  if (data) {
    GLenum format = (_channels == 3) ? GL_RGB : GL_RGBA;
    glGenTextures(1, &_ID);
    glBindTexture(GL_TEXTURE_2D, _ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
    LogInfo("Can't get image data from path: " + path + ", generate white texture");
    unsigned char white[] = {255, 255, 255};
    glGenTextures(1, &_ID);
    glBindTexture(GL_TEXTURE_2D, _ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 white);
  }
}

void Texture::Bind(const unsigned int slot) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, _ID);
}
