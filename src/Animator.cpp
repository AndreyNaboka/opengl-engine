#include "Animator.h"
#include "Utils/Logger.h"
#include <algorithm>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

void Animator::SetAnimation(Animation *anim,
                            const std::vector<glm::mat4> &inverseBindMatrices,
                            const std::vector<int> &boneParents,
                            const std::vector<std::string> &boneNames,
                            bool loop) {
  _currentAnim = anim;
  _boneOffsets = inverseBindMatrices;
  _boneParents = boneParents;
  _boneNames = boneNames;
  _loop = loop;
  _time = 0.0f;
  if (anim) {
    if (anim->duration <= 0.0f) {
      _currentAnim->duration = 1.0f; // чтобы избежать деления на 0
    }
    _finalBoneMatrices.resize(_boneParents.size(), glm::mat4(1.0f));
  }
}

void Animator::Update(float dt) {
  if (!_currentAnim)
    return;
  _time += dt;
  if (_loop && _time > _currentAnim->duration)
    _time = std::fmod(_time, _currentAnim->duration);

  std::vector<glm::mat4> localTransforms(_boneParents.size(), glm::mat4(1.0f));
  for (size_t i = 0; i < _boneParents.size(); ++i) {
    const BoneChannel *channel = nullptr;
    for (const auto &ch : _currentAnim->channels) {
      if (ch.boneName == _boneNames[i]) {
        channel = &ch;
        break;
      }
    }
    if (channel && !channel->keyframes.empty()) {
      Keyframe kf = Interpolate(channel->keyframes, _time);
      localTransforms[i] = TransformFromKeyframe(kf);
    }
  }

  std::vector<glm::mat4> globalTransforms(_boneParents.size());
  for (size_t i = 0; i < _boneParents.size(); ++i) {
    if (_boneParents[i] == -1)
      globalTransforms[i] = localTransforms[i];
    else
      globalTransforms[i] =
          globalTransforms[_boneParents[i]] * localTransforms[i];
  }

  for (size_t i = 0; i < _boneParents.size(); ++i) {
    if (i < _boneOffsets.size())
      _finalBoneMatrices[i] = globalTransforms[i] * _boneOffsets[i];
    else
      _finalBoneMatrices[i] = globalTransforms[i];
  }
}

Keyframe Animator::Interpolate(const std::vector<Keyframe> &keys, float time) {
  if (keys.empty())
    return Keyframe{time};
  if (keys.size() == 1)
    return keys[0];
  auto it =
      std::lower_bound(keys.begin(), keys.end(), time,
                       [](const Keyframe &k, float t) { return k.time < t; });
  if (it == keys.begin())
    return keys[0];
  if (it == keys.end())
    return keys.back();
  const Keyframe &k0 = *(it - 1);
  const Keyframe &k1 = *it;
  float t = (time - k0.time) / (k1.time - k0.time);
  t = glm::clamp(t, 0.0f, 1.0f);
  Keyframe result{time};
  if (k0.hasPosition && k1.hasPosition) {
    result.position = glm::mix(k0.position, k1.position, t);
    result.hasPosition = true;
  }
  if (k0.hasRotation && k1.hasRotation) {
    result.rotation = glm::slerp(k0.rotation, k1.rotation, t);
    result.hasRotation = true;
  }
  if (k0.hasScale && k1.hasScale) {
    result.scale = glm::mix(k0.scale, k1.scale, t);
    result.hasScale = true;
  }
  return result;
}

glm::mat4 Animator::TransformFromKeyframe(const Keyframe &kf) {
  glm::mat4 mat = glm::translate(glm::mat4(1.0f), kf.position);
  mat = mat * glm::mat4_cast(kf.rotation);
  mat = glm::scale(mat, kf.scale);
  return mat;
}
