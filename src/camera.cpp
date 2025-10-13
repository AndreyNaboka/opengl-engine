#include "camera.h"
#include "settings.h"
#include "glad/glad.h"

camera::camera()
{
   _pos   = glm::vec3(0.0f, 0.0f, 3.0f);
   _front = glm::vec3(0.0f, 0.0f, -1.0f);
   _up    = glm::vec3(0.0f, 1.0f, 0.0f);

   _proj_matrix = glm::perspective(45.0f, WINDOW_ASPECT_RATIO, 0.1f, 100.0f);
}

void camera::move_left()
{
}

void camera::move_right()
{
}

void camera::move_forward()
{
}

void camera::move_backward()
{
}

void camera::mouse_move(const float x, const float y, bool constrain_pitch)
{
   if (x == 0.0f && y == 0.0f) return;

   _yaw += x * _mouse_sensitivity;
   _pitch += y * _mouse_sensitivity;

   if (constrain_pitch) _pitch = std::clamp(_pitch, -89.0f, 89.0f);
}

void camera::mouse_scroll(const float x_offset, const float y_offset)
{
   if (_zoom >= 1.0f && _zoom <= 45.0f) _zoom -= y_offset;
   if (_zoom < 1.0f)  _zoom = 1.0f;
   if (_zoom > 45.0f) _zoom = 45.0f;
}

void camera::update_camera_vectors()
{

}