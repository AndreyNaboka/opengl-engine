#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  int boneIds[4] = {-1, -1, -1, -1};
  float boneWeights[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  Vertex() = default;

  Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex)
      : position(pos), normal(norm), uv(tex) {}
};

class Mesh {
public:
  Mesh(const std::vector<Vertex> &vertices,
       const std::vector<unsigned int> &indices);
  ~Mesh();
  void Draw() const;

private:
  unsigned int _VAO, _VBO, _EBO, _count;
};
