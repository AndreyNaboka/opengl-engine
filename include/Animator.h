#pragma once
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
  std::vector<glm::mat4> _finalBoneMatrices;
  float _time = 0.0f;
  bool _loop = true;

  void CalculateBoneTransform(int boneIndex, const glm::mat4 &parentTransform,
                              float time);
  Keyframe Interpolate(const std::vector<Keyframe> &keys, float time);
  glm::mat4 TransformFromKeyframe(const Keyframe &kf);
};
