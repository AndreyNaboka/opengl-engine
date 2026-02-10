#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
   enum class CameraDirection
   {
      LEFT,
      RIGHT,
      FORWARD,
      BACKWARD
   };

public:
   Camera(const glm::vec3 &pos);
   void SetSpeed(const float newSpeed) { _speed = newSpeed; }
   void SetMouseSensitivity(const float mouseSensitivity) { _mouseSensitivity = mouseSensitivity; }
   void MouseMove(const float x, const float y, bool constrainPitch = true);
   void SetDeltaTime(const float dt) { _deltaTime = dt; };

   const glm::mat4 &GetProjMatrix() const { return _projMatrix; }
   const glm::mat4 &GetViewMatrix();
   const glm::vec3 &GetPos() const { return _pos; }
   void MoveCamera(const Camera::CameraDirection dir);

private:
   void UpdateCameraVectors();

private:
   float _yaw = -90.0f;
   float _pitch = 0.0f;
   float _zoom = 45.0f;
   float _speed = 6.0f;
   float _mouseSensitivity = 0.15f;

   bool _needUpdateMatrix = false;

   glm::mat4 _viewMatrix;
   glm::mat4 _projMatrix;
   glm::vec3 _pos;
   glm::vec3 _front;
   glm::vec3 _up;
   glm::vec3 _worldUp;
   glm::vec3 _right;

   bool _firstMouseMove = true;
   float _lastXMouse = 0.0f;
   float _lastYMouse = 0.0f;

   bool _moveLeft = false;
   bool _moveRight = false;
   bool _moveForward = false;
   bool _moveBackward = false;

   float _deltaTime = 0.0f;
};