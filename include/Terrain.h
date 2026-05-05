#pragma once

#include "RenderObject.h"
#include <glad/gl.h>
#include <memory>

class Terrain {
public:
  static std::unique_ptr<RenderObject>
  CreateTerrain(const float width, const float depth, const int segX,
                const int segZ, const float texScale);
};
