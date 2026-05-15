#include "Mesh.h"
#include <glad/gl.h>

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<unsigned int> &indices)
    : _count(indices.size()) {
  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);

  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
               indices.data(), GL_STATIC_DRAW);

  // loc 0: position (3 floats)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  // loc 1: normal (3 floats)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  // loc 2: uv (2 floats)
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, uv));
  glEnableVertexAttribArray(2);

  // loc 3: boneIds (4 ints)
  glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex),
                         (void *)offsetof(Vertex, boneIds));
  glEnableVertexAttribArray(3);

  // loc 4: boneWeights (4 floats)
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, boneWeights));
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
  glDrawElements(GL_TRIANGLES, _count, GL_UNSIGNED_INT, 0);
}
