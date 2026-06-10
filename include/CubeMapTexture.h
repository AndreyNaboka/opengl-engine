#pragma once
#include "BindableTexture.h"
#include <string>
#include <array>
#include <glad/gl.h>

class CubeMapTexture : public BindableTexture {
public:
  explicit CubeMapTexture(const std::array<std::string, 6> &faces);
  ~CubeMapTexture();
  CubeMapTexture(const CubeMapTexture &) = delete;
  CubeMapTexture &operator=(const CubeMapTexture &) = delete;

  void Bind(unsigned int slot = 0) const override;
  unsigned int GetID() const override { return _ID; }
  unsigned int GetTarget() const override;

private:
  unsigned int _ID;
};
