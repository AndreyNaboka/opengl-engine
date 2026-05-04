#include "Terrain.h"
#include "RenderObject.h"
#include <memory>

std::unique_ptr<RenderObject> Terrain::CreateTerrain() {
  std::vector<unsigned int> indices;
  std::vector<float> vertices;
  return std::make_unique<RenderObject>(vertices, indices);
}
