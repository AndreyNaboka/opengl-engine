#include "Camera.h"
#include "InputManager.h"
#include "Utils/Logger.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <stdio.h>
#include <stdlib.h>

Camera::Camera(glm::vec3 position, float fov, float aspect)
    : _position(position), _front(0.0f, 0.0f, -1.0f), _up(0.0f, 1.0f, 0.0f),
      _worldUp(0.0f, 1.0f, 0.0f), _yaw(-90.0f), _pitch(0.0f),
      _movementSpeed(12.0f), _mouseSensitivity(0.15f), _FOV(fov),
      _aspect(aspect), _nearPlane(0.1f), _farPlane(1000.0f) {
  UpdateVectors();
}

void Camera::Update(const InputManager &input, float deltaTime) {
  const float velocity = _movementSpeed * deltaTime;

  if (input.IsKeyPressed(GLFW_KEY_W))
    _position += _front * velocity;
  if (input.IsKeyPressed(GLFW_KEY_S))
    _position -= _front * velocity;
  if (input.IsKeyPressed(GLFW_KEY_A))
    _position -= _right * velocity;
  if (input.IsKeyPressed(GLFW_KEY_D))
    _position += _right * velocity;
  if (input.IsKeyPressed(GLFW_KEY_SPACE))
    _position += _worldUp * velocity;
  if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
    _position -= _worldUp * velocity;

  float dx, dy;
  input.GetMouseDelta(dx, dy);
  dx *= _mouseSensitivity;
  dy *= _mouseSensitivity;

  _yaw += dx;
  _pitch += dy;
  _pitch = glm::clamp(_pitch, -89.0f, 89.0f);

  UpdateVectors();
}

std::string Camera::GetDebugStringPos() const {
  char cameraPos[1024];
  snprintf(cameraPos, 1024, "[Camera] pos: %f,%f,%f", _position.x, _position.y,
           _position.z);
  return std::string(cameraPos);
}

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(_position, _position + _front, _up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
  return glm::perspective(glm::radians(_FOV), _aspect, _nearPlane, _farPlane);
}

void Camera::UpdateVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  front.y = sin(glm::radians(_pitch));
  front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  _front = glm::normalize(front);
  _right = glm::normalize(glm::cross(_front, _worldUp));
  _up = glm::normalize(glm::cross(_right, _front));
}
