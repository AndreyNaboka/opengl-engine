#pragma once
#include <cstddef>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Animation.h"

class Animator {
public:
  void SetAnimation(Animation *anim,
                    const std::vector<glm::mat4> &inverseBindMatrices,
                    const std::vector<int> &boneParents,
                    const std::vector<std::string> &boneNames,
                    const std::vector<glm::vec3> &bindTranslations,
                    const std::vector<glm::quat> &bindRotations,
                    const std::vector<glm::vec3> &bindScales,
                    const std::vector<glm::mat4> &rootParentTransforms,
                    bool loop = true);
  void Update(float dt);
  const std::vector<glm::mat4> &GetBoneMatrices() const {
    return _finalBoneMatrices;
  }
  bool IsPlaying() const { return _currentAnim != nullptr; }

private:
  Animation *_currentAnim = nullptr;
  std::vector<glm::mat4> _boneOffsets;
  std::vector<int> _boneParents;
  std::vector<std::string> _boneNames;
  std::vector<glm::vec3> _bindTranslations;
  std::vector<glm::quat> _bindRotations;
  std::vector<glm::vec3> _bindScales;
  std::vector<glm::mat4> _rootParentTransforms;
  std::vector<glm::mat4> _finalBoneMatrices;
  float _time = 0.0f;
  bool _loop = true;

  void CalculateBoneTransform(size_t boneIndex,
                              const glm::mat4 &parentTransform,
                              const std::vector<const BoneChannel *> &channels);
  Keyframe Interpolate(const std::vector<Keyframe> &keys, float time);
  glm::mat4 TransformFromChannel(size_t boneIndex, const BoneChannel *channel);
};
