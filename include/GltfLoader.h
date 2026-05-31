#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <glm/matrix.hpp>
#include <cgltf.h>
#include "Mesh.h"
#include "Animation.h"
#include "Texture.h"

struct GltfModelData {
  struct Material {
    std::shared_ptr<Texture> albedoTexture;
    glm::vec3 baseColor = glm::vec3(1.0f);
    float mettalic = 0.0f;
    float roughness = 0.f;
  };

  std::shared_ptr<Mesh> mesh;
  std::vector<std::shared_ptr<Animation>> animations;
  std::vector<glm::mat4> inverseBindMatrices;
  std::unordered_map<std::string, size_t> boneNameToIndex;
  bool isSkinned = false;
  std::vector<Material> materials;
  int defaultMaterialsIndex = -1;
};

class GltfLoader {
public:
  // Статический метод загрузки. Возвращает shared-данные модели.
  static GltfModelData Load(const std::string &assetPath);
  static std::shared_ptr<Texture>
  LoadTextureFromCgltf(const cgltf_image *image, const std::string &basePath);
  static GltfModelData::Material LoadMaterial(const cgltf_material *material,
                                              const std::string &basePath);
};
