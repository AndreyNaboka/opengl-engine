#pragma once
#include "Mesh.h"
#include <memory>

class Sky {
public:
  static std::shared_ptr<Mesh> CreateSkyBoxMesh();
};
