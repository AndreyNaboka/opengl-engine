#include "Camera.h"
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(const glm::vec3 pos, const glm::vec3 up, const float yaw,
               const float pitch)
    : _position(pos), _worldUp(up), _yaw(yaw), _pitch(pitch) {
  UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  front.y = sin(glm::radians(_pitch));
  front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  _front = glm::normalize(front);
  _right = glm::normalize(glm::cross(_front, _worldUp));
  _up = glm::normalize(glm::cross(_right, _front));
}

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(_position, _position + _front, _up);
}

void Camera::ProcessKeyboard(const int direction, const float deltaTime) {
  const float velocity = _movementSpeed * deltaTime;
  if (direction == 1)
    _position += _front * velocity; // Forward (W)
  if (direction == 2)
    _position -= _front * velocity; // Backward (S)
  if (direction == 3)
    _position -= _right * velocity; // Left (A)
  if (direction == 4)
    _position += _right * velocity; // Right (D)
  if (direction == 5)
    _position += _worldUp * velocity; // Up (Space)
  if (direction == 6)
    _position -= _worldUp * velocity; // Down (Shift);
}
