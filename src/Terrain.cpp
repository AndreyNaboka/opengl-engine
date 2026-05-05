#include "Terrain.h"
#include "RenderObject.h"
#include <memory>

std::unique_ptr<RenderObject>
Terrain::CreateTerrain(const float width, const float depth, const int segX,
                       const int segZ, const float texScale) {

  constexpr size_t VERTEX_STRIDE = 8; // 3 pos + 3 normal + 2 uv

  const float halfW = width * 0.5f;
  const float halfD = depth * 0.5f;
  const float stepW = width / static_cast<float>(segX);
  const float stepD = depth / static_cast<float>(segZ);
  std::vector<unsigned int> indices;
  std::vector<float> vertices;

  vertices.reserve((segX + 1) * (segZ + 1) * VERTEX_STRIDE);
  size_t offset = 0;

  for (int z = 0; z <= segZ; z++) {
    for (int x = 0; x <= segX; x++) {
      const float xPos = (x * stepW) - halfW;
      const float zPos = (z * stepD) - halfD;
      // position
      vertices[offset++] = xPos;
      vertices[offset++] = 0.0f;
      vertices[offset++] = zPos;
      // normal
      vertices[offset++] = 0.0f;
      vertices[offset++] = 1.0f;
      vertices[offset++] = 0.0f;
      // UV
      vertices[offset++] = static_cast<float>(x) / segX * texScale;
      vertices[offset++] = static_cast<float>(z) / segZ * texScale;
    }
  }

  indices.reserve(segX * segZ * 6); // 2 треугольника по 3 индекса на клетку
  for (int z = 0; z < segZ; z++) {
    for (int x = 0; x < segX; x++) {
      const int i0 = z * (segX + 1) + x;
      const int i1 = i0 + 1;
      const int i2 = (z + 1) * (segX + 1) + x;
      const int i3 = i2 + 1;

      indices.push_back(i0);
      indices.push_back(i2);
      indices.push_back(i1);
      indices.push_back(i1);
      indices.push_back(i2);
      indices.push_back(i3);
    }
  }

  return std::make_unique<RenderObject>(std::move(vertices),
                                        std::move(indices));
}
