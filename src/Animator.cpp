#include "Animator.h"
#include <algorithm>
#include <cmath>

void Animator::SetAnimation(Animation *anim,
                            const std::vector<glm::mat4> &inverseBindMatrices,
                            const std::vector<int> &boneParents,
                            const std::vector<std::string> &boneNames,
                            const std::vector<glm::vec3> &bindTranslations,
                            const std::vector<glm::quat> &bindRotations,
                            const std::vector<glm::vec3> &bindScales,
                            const std::vector<glm::mat4> &rootParentTransforms,
                            bool loop) {
  _currentAnim = anim;
  _boneOffsets = inverseBindMatrices;
  _boneParents = boneParents;
  _boneNames = boneNames;
  _bindTranslations = bindTranslations;
  _bindRotations = bindRotations;
  _bindScales = bindScales;
  _rootParentTransforms = rootParentTransforms;
  _loop = loop;
  _time = 0.0f;
  _channelsByBone.assign(_boneParents.size(), nullptr);
  _childrenByBone.assign(_boneParents.size(), {});
  _rootBones.clear();

  if (anim) {
    if (anim->duration <= 0.0f) {
      _currentAnim->duration = 1.0f; // чтобы избежать деления на 0
    }
    _finalBoneMatrices.resize(_boneParents.size(), glm::mat4(1.0f));

    for (size_t bone = 0; bone < _boneNames.size(); ++bone) {
      for (const auto &channel : anim->channels) {
        if (channel.boneName == _boneNames[bone]) {
          _channelsByBone[bone] = &channel;
          break;
        }
      }
    }

    for (size_t bone = 0; bone < _boneParents.size(); ++bone) {
      const int parent = _boneParents[bone];
      if (parent < 0) {
        _rootBones.push_back(bone);
      } else if (static_cast<size_t>(parent) < _childrenByBone.size()) {
        _childrenByBone[parent].push_back(bone);
      }
    }
  } else {
    _finalBoneMatrices.clear();
  }
}

void Animator::Update(float dt) {
  if (!_currentAnim)
    return;
  _time += dt;
  if (_loop && _time > _currentAnim->duration)
    _time = std::fmod(_time, _currentAnim->duration);

  for (size_t root : _rootBones) {
    const glm::mat4 rootParent = root < _rootParentTransforms.size()
                                     ? _rootParentTransforms[root]
                                     : glm::mat4(1.0f);
    CalculateBoneTransform(root, rootParent);
  }
}

void Animator::CalculateBoneTransform(size_t boneIndex,
                                      const glm::mat4 &parentTransform) {
  glm::mat4 globalTransform =
      parentTransform *
      TransformFromChannel(boneIndex, _channelsByBone[boneIndex]);

  if (boneIndex < _boneOffsets.size())
    _finalBoneMatrices[boneIndex] = globalTransform * _boneOffsets[boneIndex];
  else
    _finalBoneMatrices[boneIndex] = globalTransform;

  for (size_t child : _childrenByBone[boneIndex]) {
    CalculateBoneTransform(child, globalTransform);
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

glm::mat4 Animator::TransformFromChannel(size_t boneIndex,
                                         const BoneChannel *channel) {
  glm::vec3 position = boneIndex < _bindTranslations.size()
                           ? _bindTranslations[boneIndex]
                           : glm::vec3(0.0f);
  glm::quat rotation = boneIndex < _bindRotations.size()
                           ? _bindRotations[boneIndex]
                           : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 scale =
      boneIndex < _bindScales.size() ? _bindScales[boneIndex] : glm::vec3(1.0f);

  if (channel) {
    Keyframe positionKey = Interpolate(channel->positionKeys, _time);
    Keyframe rotationKey = Interpolate(channel->rotationKeys, _time);
    Keyframe scaleKey = Interpolate(channel->scaleKeys, _time);
    if (positionKey.hasPosition)
      position = positionKey.position;
    if (rotationKey.hasRotation)
      rotation = rotationKey.rotation;
    if (scaleKey.hasScale)
      scale = scaleKey.scale;
  }

  glm::mat4 mat = glm::translate(glm::mat4(1.0f), position);
  mat = mat * glm::mat4_cast(rotation);
  mat = glm::scale(mat, scale);
  return mat;
}
