#include "Mesh.h"
#include <glad/gl.h>
#include <cstddef>

Mesh::Mesh(const std::vector<StaticVertex> &vertices,
           const std::vector<uint32_t> &indices)
    : _count(static_cast<unsigned int>(indices.size())),
      _indexType(GL_UNSIGNED_INT) {

  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);

  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StaticVertex),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
               indices.data(), GL_STATIC_DRAW);

  constexpr GLsizei stride = sizeof(StaticVertex); // 32 байта
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(StaticVertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(StaticVertex, normal));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(StaticVertex, uv));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

// Скинненный меш: 5 атрибутов, stride = 64 байта
Mesh::Mesh(const std::vector<SkinnedVertex> &vertices,
           const std::vector<uint16_t> &indices)
    : _count(static_cast<unsigned int>(indices.size())),
      _indexType(GL_UNSIGNED_SHORT) {

  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);

  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinnedVertex),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t),
               indices.data(), GL_STATIC_DRAW);

  constexpr GLsizei stride = sizeof(SkinnedVertex); // 64 байта
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(SkinnedVertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(SkinnedVertex, normal));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(SkinnedVertex, uv));
  glEnableVertexAttribArray(2);
  glVertexAttribIPointer(3, 4, GL_INT, stride,
                         (void *)offsetof(SkinnedVertex, boneIds));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(SkinnedVertex, boneWeights));
  glEnableVertexAttribArray(4);

  glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<SkinnedVertex> &vertices,
           const std::vector<uint32_t> &indices)
    : _count(static_cast<unsigned int>(indices.size())),
      _indexType(GL_UNSIGNED_INT) {
  // Аналогично uint16 версии, но с GL_UNSIGNED_INT
  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);
  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinnedVertex),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
               indices.data(), GL_STATIC_DRAW);
  constexpr GLsizei stride = sizeof(SkinnedVertex);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(SkinnedVertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(SkinnedVertex, normal));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(SkinnedVertex, uv));
  glEnableVertexAttribArray(2);
  glVertexAttribIPointer(3, 4, GL_INT, stride,
                         (void *)offsetof(SkinnedVertex, boneIds));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(SkinnedVertex, boneWeights));
  glEnableVertexAttribArray(4);
  glBindVertexArray(0);
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &_VAO);
  glDeleteBuffers(1, &_VBO);
  glDeleteBuffers(1, &_EBO);
}

void Mesh::Draw() const {
  glBindVertexArray(_VAO);
  glDrawElements(GL_TRIANGLES, _count, _indexType, nullptr);
  glBindVertexArray(0);
}
