#pragma once
#include <vector>

struct Vertex {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
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
