#pragma once
#include <string>

class Texture {
public:
  Texture(const std::string &pathToTexture);
  void Bind(const unsigned int slot = 0);
	unsigned int GetID() const { return _ID; }

private:
  int _channels = 0;
  int _width = 0;
  int _height = 0;
  unsigned int _ID = 0;
};
