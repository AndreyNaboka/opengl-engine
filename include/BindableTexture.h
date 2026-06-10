#pragma once

class BindableTexture {
public:
  virtual ~BindableTexture() = default;
  virtual void Bind(unsigned int slot = 0) const = 0;
  virtual unsigned int GetID() const = 0;
  virtual unsigned int GetTarget() const = 0;
};
