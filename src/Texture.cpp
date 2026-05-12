#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/gl.h>
#include "Utils/Logger.h"
#include "Utils/PathUtils.h"

Texture::Texture(const std::string &path) {
  stbi_set_flip_vertically_on_load(true);
  auto absolutePath = Path::ResolveAssetPath(path);
  unsigned char *data =
      stbi_load(absolutePath.string().c_str(), &_width, &_height, nullptr, 4);
  LogInfo("[Texture] Try to load texture from: " + absolutePath.string());
  if (!data) {
    LogInfo("[Texture] Failed to load: " + path);
    _width = _height = 1;
    unsigned char white[] = {255, 255, 255, 255};
    glGenTextures(1, &_ID);
    glBindTexture(GL_TEXTURE_2D, _ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 white);
    return;
  }

  glGenTextures(1, &_ID);
  glBindTexture(GL_TEXTURE_2D, _ID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_image_free(data);
}

Texture::~Texture() { glDeleteTextures(1, &_ID); }
void Texture::Bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, _ID);
}
