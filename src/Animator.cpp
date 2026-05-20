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
  Keyframe kf;
  return kf;
}
glm::mat4 Animator::TransformFromKeyframe(const Keyframe &kf) {
  glm::mat4 m;
  return m;
}
