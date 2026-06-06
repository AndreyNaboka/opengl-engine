#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <unordered_map>

struct Keyframe {
  float time = 0.0f;
  glm::vec3 position{0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f};
  bool hasPosition = false; // Флаги присутствия данных в этом ключе
  bool hasRotation = false;
  bool hasScale = false;
};

struct BoneChannel {
  std::string boneName;
  std::vector<Keyframe> positionKeys;
  std::vector<Keyframe> rotationKeys;
  std::vector<Keyframe> scaleKeys;
};

class Animation {
public:
  std::string name;
  float duration = 0.0f;             // Общая длительность в секундах
  std::vector<BoneChannel> channels; // Каналы по костям
  std::unordered_map<std::string, size_t> boneIndexMap;
};
