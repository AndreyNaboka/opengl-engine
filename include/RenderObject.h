#pragma once
#include <vector>
class RenderObject {
public:
  RenderObject(const std::vector<float> &vertices,
               const std::vector<unsigned int> &indeces);
  void Draw();

private:
  unsigned int _VAO = 0;
  unsigned int _VBO = 0;
  unsigned int _EBO = 0;
  unsigned int _indexCount = 0;
};
