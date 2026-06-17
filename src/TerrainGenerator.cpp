#include "TerrainGenerator.h"
#include "Mesh.h"
#include <algorithm>
#include <cmath>

namespace {
float SmoothStep(float edge0, float edge1, float value) {
  const float t = std::clamp((value - edge0) / (edge1 - edge0), 0.0f, 1.0f);
  return t * t * (3.0f - 2.0f * t);
}

} // namespace

float SampleTerrainHeight(float x, float z, float width, float depth) {
  const float halfWidth = width * 0.5f;
  const float halfDepth = depth * 0.5f;
  const float nx = std::abs(x) / halfWidth;
  const float nz = std::abs(z) / halfDepth;
  const float edgeDistance = std::max(nx, nz);
  const float edgeMask = SmoothStep(0.58f, 0.98f, edgeDistance);

  const float smallBumps =
      std::sin(x * 0.12f) * 0.45f + std::cos(z * 0.10f) * 0.35f +
      std::sin((x + z) * 0.055f) * 0.30f;
  const float mountainDetail =
      std::sin(x * 0.075f + z * 0.035f) * 3.0f +
      std::cos(z * 0.095f - x * 0.025f) * 2.5f;

  return smallBumps + edgeMask * (8.0f + edgeMask * 20.0f + mountainDetail);
}

std::unique_ptr<Mesh> GenerateGrid(float width, float depth, unsigned int resX,
                                   unsigned int resZ, float uvScale) {
  std::vector<StaticVertex> vertices;
  std::vector<uint32_t> indices;
  vertices.reserve((resX + 1) * (resZ + 1));

  float hw = width * 0.5f, hd = depth * 0.5f;
  float sx = width / resX, sz = depth / resZ;

  for (unsigned int z = 0; z <= resZ; ++z) {
    for (unsigned int x = 0; x <= resX; ++x) {
      const float px = x * sx - hw;
      const float pz = z * sz - hd;
      const float py = SampleTerrainHeight(px, pz, width, depth);

      vertices.emplace_back(glm::vec3(px, py, pz),
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

  for (StaticVertex &vertex : vertices) {
    vertex.normal = glm::vec3(0.0f);
  }

  for (std::size_t i = 0; i < indices.size(); i += 3) {
    StaticVertex &a = vertices[indices[i]];
    StaticVertex &b = vertices[indices[i + 1]];
    StaticVertex &c = vertices[indices[i + 2]];
    const glm::vec3 normal = glm::normalize(
        glm::cross(b.position - a.position, c.position - a.position));
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  for (StaticVertex &vertex : vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }

  return std::make_unique<Mesh>(vertices, indices);
}
