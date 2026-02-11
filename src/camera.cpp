#include "Camera.h"
#include "Settings.h"
#include "glad/glad.h"
#include <glfw/glfw3.h>

Camera::Camera(const glm::vec3 &pos)
{
   _pos = pos;
   _up = glm::vec3(0.0f, 1.0f, 0.0f);
   _front = glm::vec3(0.0f, 0.0f, -1.0f);
   _projMatrix = glm::perspective(45.0f, WINDOW_ASPECT_RATIO, 0.1f, 100.0f);
   _worldUp = _up;
   _viewMatrix = glm::mat4(1.0f);
   UpdateCameraVectors();
}

void Camera::MoveCamera(const Camera::CameraDirection dir)
{
   const float velocity = _deltaTime * _speed;

   switch (dir)
   {
   case Camera::CameraDirection::FORWARD:
      _pos += _front * velocity;
      break;
   case Camera::CameraDirection::BACKWARD:
      _pos -= _front * velocity;
      break;
   case Camera::CameraDirection::LEFT:
      _pos -= _right * velocity;
      break;
   case Camera::CameraDirection::RIGHT:
      _pos += _right * velocity;
      break;
   default:
      break;
   }

   UpdateCameraVectors();
}

void Camera::MouseMove(const float x, const float y, bool constrainPitch)
{
   _yaw += x * _mouseSensitivity;
   _pitch += y * _mouseSensitivity;

   if (constrainPitch)
      _pitch = std::clamp(_pitch, -89.0f, 89.0f);

   UpdateCameraVectors();
}

const glm::mat4 &Camera::GetViewMatrix()
{
   if (_needUpdateMatrix)
   {
      _viewMatrix = glm::lookAt(_pos, _pos + _front, _up);
      _needUpdateMatrix = false;
   }
   return _viewMatrix;
}

void Camera::UpdateCameraVectors()
{
   _front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
   _front.y = sin(glm::radians(_pitch));
   _front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
   _front = glm::normalize(_front);
   _right = glm::normalize(glm::cross(_front, _worldUp));
   _up = glm::normalize(glm::cross(_right, _front));
   _needUpdateMatrix = true;
}
