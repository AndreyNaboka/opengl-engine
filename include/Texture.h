#pragma once
#include <string>
#include <cstdint>

class Texture {
public:
  explicit Texture(const std::string &path);
  Texture(const uint8_t *data, size_t size);
  ~Texture();
  void Bind(unsigned int slot = 0) const;
  int GetWidth() const { return _width; }
  int GetHeight() const { return _height; }

private:
  unsigned int _ID = 0;
  int _width = 0;
  int _height = 0;
};
