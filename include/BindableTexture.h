#pragma once

class BindableTexture {
public:
  virtual ~BindableTexture() = default;
  virtual void Bind(unsigned int slot = 0) const = 0;
};
