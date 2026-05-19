#pragma once
#include "Animation.h"
#include <glm/fwd.hpp>
#include <vector>
#include <glm/glm.hpp>

class Animator {
public:
  void SetAnimation(const Animation *anim,
                    const std::vector<glm::mat4> &inverseBindMatrices,
                    bool loop = true);
  void Update(float dt);
  const std::vector<glm::mat4> &GetBoneMatrices() const {
    return _finalBoneMatrices;
  }
  bool IsPlaying() const { return _currentAnim != nullptr; }

private:
  void CalculateBoneTransforms(size_t boneIndex,
                               const glm::mat4 &parentTransform, float ime);
  Keyframe Interpolate(const std::vector<Keyframe> &keys, float time);
  glm::mat4 TransformFromKeyframe(const Keyframe *kf);

private:
  const Animation *_currentAnim = nullptr;
  std::vector<glm::mat4> _finalBoneMatrices;
  std::vector<glm::mat4> _boneOffsets;
  float _time = 0.0f;
  bool _loop = true;
};
