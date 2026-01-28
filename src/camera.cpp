#include "game.h"
#include "camera.h"
#include "settings.h"
#include "glad/glad.h"
#include <glfw/glfw3.h>

camera::camera()
{
   _pos = glm::vec3(0.0f, 0.0f, 3.0f);
   _up = glm::vec3(0.0f, 1.0f, 0.0f);
   _front = glm::vec3(0.0f, 0.0f, -1.0f);
   _proj_matrix = glm::perspective(45.0f, WINDOW_ASPECT_RATIO, 0.1f, 100.0f);
   _world_up = _up;
   _view_matrix = glm::mat4(1.0f);
   update_camera_vectors();
}

void camera::move_camera(const camera::camera_direction dir)
{
   const float velocity = _speed * game::instance().get_delta_time();

   switch (dir)
   {
   case camera::camera_direction::FORWARD:
      _pos += _front * velocity;
      break;
   case camera::camera_direction::BACKWARD:
      _pos -= _front * velocity;
      break;
   case camera::camera_direction::LEFT:
      _pos -= _right * velocity;
      break;
   case camera::camera_direction::RIGHT:
      _pos += _right * velocity;
      break;
   default:
      break;
   }

   update_camera_vectors();
}

void camera::mouse_move(const float x, const float y, bool constrain_pitch)
{
   if (_first_mouse_move)
   {
      _last_x_mouse = x;
      _last_y_mouse = y;
      _first_mouse_move = false;
   }
   const float x_offset = x - _last_x_mouse;
   const float y_offset = _last_y_mouse - y;

   _last_x_mouse = x;
   _last_y_mouse = y;

   _yaw += x * _mouse_sensitivity;
   _pitch += y * _mouse_sensitivity;

   if (constrain_pitch)
      _pitch = std::clamp(_pitch, -89.0f, 89.0f);

   update_camera_vectors();
}

const glm::mat4 &camera::get_view_matrix()
{
   if (_need_update_matrix)
   {
      _view_matrix = glm::lookAt(_pos, _pos + _front, _up);
      _need_update_matrix = false;
   }
   return _view_matrix;
}

void camera::update_camera_vectors()
{
   _front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
   _front.y = sin(glm::radians(_pitch));
   _front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
   _front = glm::normalize(_front);
   _right = glm::normalize(glm::cross(_front, _world_up));
   _up = glm::normalize(glm::cross(_right, _front));
   _need_update_matrix = true;
}
