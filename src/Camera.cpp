#include "Camera.h"

Camera::Camera(const glm::vec3 pos, const glm::vec3 up, const float yaw,
               const float pitch)
    : _position(pos), _worldUp(up), _yaw(yaw), _pitch(pitch) {
  UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {}
