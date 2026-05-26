#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

struct StaticVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
};

struct SkinnedVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  int boneIds[4];
  float boneWeights[4];
};

class Mesh {
public:
  // Для статичной геометрии
  Mesh(const std::vector<StaticVertex> &vertices,
       const std::vector<uint32_t> &indices);

  // Для скинненной геометрии
  Mesh(const std::vector<SkinnedVertex> &vertices,
       const std::vector<uint16_t> &indices);
  Mesh(const std::vector<SkinnedVertex> &vertices,
       const std::vector<uint32_t> &indices);

  ~Mesh();
  void Draw() const;

  unsigned int GetVAO() const { return m_VAO; }
  unsigned int GetIndexCount() const { return m_Count; }

private:
  unsigned int m_VAO, m_VBO, m_EBO, m_Count;
  GLenum m_IndexType;
};
