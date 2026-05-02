#pragma once
#include <glm/ext.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>

class Camera {
public:
  Camera(const glm::vec3 pos = glm::vec3(0.0f, 5.0f, 10.0f),
         const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0),
         const float yaw = -90.0f, const float pitch = 0.0f);
	glm::mat4 GetViewMatrix() const;
	void ProcessKeyboard(const int direction, const float deltaTime);

private:
	void UpdateCameraVectors();

private:
  glm::vec3 _position;
  glm::vec3 _front;
  glm::vec3 _up;
  glm::vec3 _right;
  glm::vec3 _worldUp;
  float _yaw = 0.0f;
  float _pitch = 0.0f;
  float _movementSpeed = 1.0f;
  float _mouseSensevity = 1.0f;
};
