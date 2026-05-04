#pragma once

#include "RenderObject.h"
#include <glad/gl.h>
#include <memory>

class Terrain {
public:
  static std::unique_ptr<RenderObject> CreateTerrain();
};
