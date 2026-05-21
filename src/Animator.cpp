#include "Animator.h"
#include <cmath>
#include <algorithm>

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
                                       float time) {
  if (!_currentAnim || boneIndex >= _currentAnim->boneIndexMap.size())
    return;

  // Находим локальную трансформацию для текущей кости в данный момент времени
  Keyframe mergedKf{time};
  bool found = false;

  // Ищем канал анимации для этой кости
  for (const auto &ch : _currentAnim->channels) {
    auto it = _currentAnim->boneIndexMap.find(ch.boneName);
    if (it != _currentAnim->boneIndexMap.end() && it->second == boneIndex) {
      Keyframe local = Interpolate(ch.keyframes, time); // Семплируем T/R/S
      if (local.hasPosition) {
        mergedKf.position = local.position;
        mergedKf.hasPosition = true;
      }
      if (local.hasRotation) {
        mergedKf.rotation = local.rotation;
        mergedKf.hasRotation = true;
      }
      if (local.hasScale) {
        mergedKf.scale = local.scale;
        mergedKf.hasScale = true;
      }
      found = true;
    }
  }

  // Если анимация не трогает эту кость, берём identity
  if (!found) {
    mergedKf.position = glm::vec3(0.0f);
    mergedKf.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    mergedKf.scale = glm::vec3(1.0f);
    mergedKf.hasPosition = mergedKf.hasRotation = mergedKf.hasScale = true;
  }

  // Собираем матрицу локального пространства
  glm::mat4 nodeTransform = TransformFromKeyframe(mergedKf);
  // Переходим в глобальное пространство (умножаем на матрицу родителя)
  glm::mat4 globalTransform = parentTransform * nodeTransform;

  // Применяем Inverse Bind Matrix для приведения к пространству скиннинга
  if (boneIndex < _boneOffsets.size()) {
    _finalBoneMatrices[boneIndex] = globalTransform * _boneOffsets[boneIndex];
  }

  for (size_t child = boneIndex + 1; child < _currentAnim->boneIndexMap.size();
       ++child) {
    CalculateBoneTransforms(child, globalTransform, time);
    break;
  }
}

Keyframe Animator::Interpolate(const std::vector<Keyframe> &keys, float time) {
  if (keys.empty())
    return Keyframe{time};
  if (keys.size() == 1)
    return keys[0];

  // Бинарный поиск позиции времени
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
  if (k0.hasPosition) {
    result.position = glm::mix(k0.position, k1.position, t);
    result.hasPosition = true;
  }
  if (k0.hasRotation) {
    result.rotation = glm::slerp(k0.rotation, k1.rotation, t);
    result.hasRotation = true;
  }
  if (k0.hasScale) {
    result.scale = glm::mix(k0.scale, k1.scale, t);
    result.hasScale = true;
  }
  return result;
}

glm::mat4 Animator::TransformFromKeyframe(const Keyframe &kf) {
  glm::mat4 t = glm::translate(glm::mat4(1.0f), kf.position);
  t *= glm::mat4_cast(kf.rotation);
  t = glm::scale(t, kf.scale);
  return t;
}
