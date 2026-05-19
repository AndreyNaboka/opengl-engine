#include "Animator.h"

void Animator::SetAnimation(const Animation *anim,
                            const std::vector<glm::mat4> &inverseBindMatrices,
                            bool loop) {}

void Animator::Update(float dt) {}

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
