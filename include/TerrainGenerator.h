#pragma once
#include <memory>
#include "Mesh.h"

std::unique_ptr<Mesh> GenerateGrid(float width, float depth, unsigned int resX, unsigned int resZ,
                                   float uvScale); 
