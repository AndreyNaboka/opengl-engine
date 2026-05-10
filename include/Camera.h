#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class InputManager;

class Camera {
public:
  Camera(glm::vec3 position = {0.0f, 5.0f, 10.0f}, float fov = 45.0f,
         float aspect = 16.0f / 9.0f);

  void Update(const InputManager &input, float deltaTime);
  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const;

  const glm::vec3 &GetPosition() const { return _position; }
  const glm::vec3 &GetFront() const { return _front; }

private:
  glm::vec3 _position, _front, _up, _right, _worldUp;
  float _yaw = 0.0f;
  float _pitch = 0.0f;
  float _movementSpeed = 0.0f;
  float _mouseSensitivity = 0.0f;
  float _FOV = 0.0f;
  float _aspect = 0.0f;
  float _nearPlane = 0.0f;
  float _farPlane = 0.0f;

  void UpdateVectors();
};
