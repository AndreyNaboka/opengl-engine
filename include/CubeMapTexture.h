#pragma once
#include <string>
#include <array>
#include <glad/gl.h>

class CubeMapTexture {
public:
  explicit CubeMapTexture(const std::array<std::string, 6> &faces);
  ~CubeMapTexture();
  CubeMapTexture(const CubeMapTexture &) = delete;
  CubeMapTexture &operator=(const CubeMapTexture &) = delete;

  void Bind(unsigned int slot = 0) const;
  unsigned int GetID() const { return _ID; }

private:
  unsigned int _ID;
};
