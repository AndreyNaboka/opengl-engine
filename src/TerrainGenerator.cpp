#include "TerrainGenerator.h"

std::unique_ptr<Mesh> GenerateGrid(float width, float depth, unsigned int resX,
                                   unsigned int resZ, float uvScale) {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  vertices.reserve((resX + 1) * (resZ + 1));

  float hw = width * 0.5f, hd = depth * 0.5f;
  float sx = width / resX, sz = depth / resZ;

  for (unsigned int z = 0; z <= resZ; ++z) {
    for (unsigned int x = 0; x <= resX; ++x) {
      vertices.emplace_back(glm::vec3(x * sx - hw, 0.0f, z * sz - hd),
                            glm::vec3(0.0f, 1.0f, 0.0f),
                            glm::vec2(static_cast<float>(x) / resX * uvScale,
                                      static_cast<float>(z) / resZ * uvScale));
    }
  }

  for (unsigned int z = 0; z < resZ; ++z) {
    for (unsigned int x = 0; x < resX; ++x) {
      uint32_t tl = z * (resX + 1) + x;
      uint32_t bl = (z + 1) * (resX + 1) + x;

      // Два треугольника на квадрат
      indices.push_back(tl);
      indices.push_back(bl);
      indices.push_back(tl + 1);
      indices.push_back(tl + 1);
      indices.push_back(bl);
      indices.push_back(tl + 1 + (resX + 1));
    }
  }
  return std::make_unique<Mesh>(vertices, indices);
}
