#include "TerrainGenerator.h"

std::unique_ptr<Mesh> GenerateGrid(float width, float depth, unsigned int resX,
                                   unsigned int resZ, float uvScale) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  vertices.reserve((resX + 1) * (resZ + 1));

  float hw = width * 0.5f, hd = depth * 0.5f;
  float sx = width / resX, sz = depth / resZ;

  for (int z = 0; z <= resZ; ++z) {
    for (int x = 0; x <= resX; ++x) {
      vertices.push_back({x * sx - hw, 0.0f, z * sz - hd, 0.0f, 1.0f, 0.0f,
                          static_cast<float>(x) / resX * uvScale,
                          static_cast<float>(z) / resZ * uvScale});
    }
  }

  for (int z = 0; z < resZ; ++z) {
    for (int x = 0; x < resX; ++x) {
      unsigned int tl = z * (resX + 1) + x;
      indices.insert(indices.end(),
                     {tl, (z + 1) * (resX + 1) + x, tl + 1, tl + 1,
                      (z + 1) * (resX + 1) + x, tl + 1 + (resX + 1)});
    }
  }
  return std::make_unique<Mesh>(vertices, indices);
}
