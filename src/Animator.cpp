#include "Animator.h"
#include <cmath>

void Animator::SetAnimation(const Animation *anim,
                            const std::vector<glm::mat4> &inverseBindMatrices,
                            bool loop) {
  _currentAnim = anim;
  _boneOffsets = inverseBindMatrices;
  _loop = loop;
  _time = 0.0f;
  if (anim)
    _finalBoneMatrices.resize(anim->boneIndexMap.size(), glm::mat4(1.0f));
}

void Animator::Update(float dt) {
  if (!_currentAnim)
    return;
  _time += dt;
  if (_loop && _time > _currentAnim->duration)
    _time = std::fmod(_time, _currentAnim->duration);
  CalculateBoneTransforms(0, glm::mat4(1.0f), _time);
}

void Animator::CalculateBoneTransforms(size_t boneIndex,
                                       const glm::mat4 &parentTransform,
                                       float time) {}
Keyframe Animator::Interpolate(const std::vector<Keyframe> &keys, float time) {
  Keyframe kf;
  return kf;
}
glm::mat4 Animator::TransformFromKeyframe(const Keyframe *kf) {
  glm::mat4 m;
  return m;
}
