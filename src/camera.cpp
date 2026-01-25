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
   const float delta_time = game::instance().get_delta_time();
   const float velocity = _speed * delta_time;

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
   _yaw += x * _mouse_sensitivity;
   _pitch += y * _mouse_sensitivity;

   if (constrain_pitch)
      _pitch = std::clamp(_pitch, -89.0f, 89.0f);

   update_camera_vectors();
}

void camera::on_mouse(double xpos, double ypos)
{
   if (_first_mouse_move)
   {
      _last_x_mouse = xpos;
      _last_y_mouse = ypos;
      _first_mouse_move = false;
   }
   const float x_offset = xpos - _last_x_mouse;
   const float y_offset = _last_y_mouse - ypos;

   _last_x_mouse = xpos;
   _last_y_mouse = ypos;

   mouse_move(x_offset, y_offset);
}

void camera::on_scroll(double x_offset, double y_offset)
{
   if (_zoom >= 1.0f && _zoom <= 45.0f)
      _zoom -= y_offset;
   if (_zoom < 1.0f)
      _zoom = 1.0f;
   if (_zoom > 45.0f)
      _zoom = 45.0f;
}

void camera::update()
{
   if (_move_forward)
      move_camera(camera_direction::FORWARD);
   if (_move_backward)
      move_camera(camera_direction::BACKWARD);
   if (_move_left)
      move_camera(camera_direction::LEFT);
   if (_move_right)
      move_camera(camera_direction::RIGHT);

   _move_forward = _move_backward = _move_left = _move_right = false;
}

void camera::on_key(int code, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS && action != GLFW_REPEAT)
      return;

   if (code == GLFW_KEY_W)
      _move_forward = true;
   if (code == GLFW_KEY_S)
      _move_backward = true;
   if (code == GLFW_KEY_A)
      _move_left = true;
   if (code == GLFW_KEY_D)
      _move_right = true;
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
