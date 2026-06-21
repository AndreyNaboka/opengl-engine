#include "CubeMapTexture.h"
#include "Utils/PathUtils.h"
#include "Utils/Logger.h"
#include <algorithm>
#include <array>
#include <stb_image.h>
#include <string>
#include <vector>

namespace {
constexpr int kFaceCount = 6;
constexpr int kRequiredChannels = STBI_rgb_alpha;

const char *GlErrorName(GLenum error) {
  switch (error) {
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  default:
    return "unknown OpenGL error";
  }
}

void LogGlErrors(const std::string &operation) {
  for (GLenum error = glGetError(); error != GL_NO_ERROR;
       error = glGetError()) {
    LogInfo("[CubeMap] " + operation + " failed: " + GlErrorName(error) +
            " (" + std::to_string(error) + ")");
  }
}
} // namespace

CubeMapTexture::CubeMapTexture(const std::array<std::string, 6> &faces) {
  stbi_set_flip_vertically_on_load(false);

  glGenTextures(1, &_ID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);

  int faceWidth = 0;
  int faceHeight = 0;
  for (const std::string &face : faces) {
    const std::string path = Path::ResolveAssetPath(face).string();
    if (stbi_info(path.c_str(), &faceWidth, &faceHeight, nullptr))
      break;
  }
  if (faceWidth <= 0 || faceHeight <= 0) {
    faceWidth = 1;
    faceHeight = 1;
    LogInfo("[CubeMap] No readable faces; using 1x1 fallback cubemap");
  }

  GLint previousUnpackAlignment = 4;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  const std::array<unsigned char, 4> fallbackPixel = {255, 0, 255, 255};
  for (int i = 0; i < kFaceCount; ++i) {
    const std::string path = Path::ResolveAssetPath(faces[i]).string();
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels,
                                    kRequiredChannels);

    const bool validSize = data && width == faceWidth && height == faceHeight;
    if (validSize) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width,
                   height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      LogInfo("[CubeMap] Loaded face " + std::to_string(i) + " (" +
              std::to_string(width) + "x" + std::to_string(height) +
              ", source channels " + std::to_string(channels) + "): " +
              path);
    } else {
      std::string reason;
      if (!data) {
        const char *failure = stbi_failure_reason();
        reason = failure ? failure : "unknown stb_image error";
      } else {
        reason = "size " + std::to_string(width) + "x" +
                 std::to_string(height) + " does not match " +
                 std::to_string(faceWidth) + "x" +
                 std::to_string(faceHeight);
      }

      std::vector<unsigned char> fallback(
          static_cast<size_t>(faceWidth) * static_cast<size_t>(faceHeight) * 4);
      for (size_t pixel = 0; pixel < fallback.size(); pixel += 4)
        std::copy(fallbackPixel.begin(), fallbackPixel.end(),
                  fallback.begin() + pixel);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, faceWidth,
                   faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallback.data());
      LogInfo("[CubeMap] Failed to load face " + std::to_string(i) + " (" +
              path + "): " + reason + "; using magenta fallback");
    }

    stbi_image_free(data);
    LogGlErrors("uploading face " + std::to_string(i));
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);

  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  LogGlErrors("generating mipmaps");
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  LogGlErrors("configuring texture parameters");
}

CubeMapTexture::~CubeMapTexture() { glDeleteTextures(1, &_ID); }

void CubeMapTexture::Bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);
}

unsigned int CubeMapTexture::GetTarget() const { return GL_TEXTURE_CUBE_MAP; }
