#pragma once
#include <memory>
#include "Mesh.h"

float SampleTerrainHeight(float x, float z, float width, float depth);
std::unique_ptr<Mesh> GenerateGrid(float width, float depth, unsigned int resX,
                                   unsigned int resZ, float uvScale);
