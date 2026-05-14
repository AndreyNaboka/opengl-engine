#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <glm/matrix.hpp>
#include "Mesh.h"
#include "Animation.h"

struct GltfModelData {
  std::shared_ptr<Mesh> mesh;
  std::vector<std::shared_ptr<Animation>> animations;
  std::vector<glm::mat4> inverseBindMatrices;
  std::unordered_map<std::string, size_t> boneToIndex;
  bool isSkinned = false;
};

class GltfLoader {
public:
  // Статический метод загрузки. Возвращает shared-данные модели.
  static GltfModelData Load(const std::string &assetPath);
};
