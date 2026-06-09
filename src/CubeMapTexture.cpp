#include "CubeMapTexture.h"
#include "Utils/PathUtils.h"
#include "Utils/Logger.h"
#include <stb_image.h>

CubeMapTexture::CubeMapTexture(const std::array<std::string, 6> &faces) {
  stbi_set_flip_vertically_on_load(false);

  glGenTextures(1, &_ID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);

  int width, height, channels;
  for (unsigned int i = 0; i < 6; ++i) {
    std::string path = Path::ResolveAssetPath(faces[i]).string();
    unsigned char *data =
        stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data) {
      GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height,
                   0, format, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
      LogInfo("[CubeMap] Loaded face " + std::to_string(i) + " : " + path);
    } else {
      LogInfo("[CubeMap] Failed to load face " + std::to_string(i) + " : " +
              path);
    }
  }

  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

CubeMapTexture::~CubeMapTexture() { glDeleteTextures(1, &_ID); }

void CubeMapTexture::Bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);
}
