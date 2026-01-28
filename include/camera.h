#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class camera
{
public:
   enum class camera_direction
   {
      LEFT,
      RIGHT,
      FORWARD,
      BACKWARD
   };

public:
   camera();
   void set_speed(const float new_speed) { _speed = new_speed; }
   void set_mouse_sensitivity(const float mouse_sensitivity) { _mouse_sensitivity = mouse_sensitivity; }
   void mouse_move(const float x, const float y, bool constrain_pitch = true);
   void update();

   const glm::mat4 &get_proj_matrix() const { return _proj_matrix; }
   const glm::mat4 &get_view_matrix();
   void move_camera(const camera::camera_direction dir);

private:
   void update_camera_vectors();

private:
   float _yaw = -90.0f;
   float _pitch = 0.0f;
   float _zoom = 45.0f;
   float _speed = 3.0f;
   float _mouse_sensitivity = 0.15f;

   bool _need_update_matrix = false;

   glm::mat4 _view_matrix;
   glm::mat4 _proj_matrix;
   glm::vec3 _pos;
   glm::vec3 _front;
   glm::vec3 _up;
   glm::vec3 _world_up;
   glm::vec3 _right;

   bool _first_mouse_move = true;
   float _last_x_mouse = 0.0f;
   float _last_y_mouse = 0.0f;

   bool _move_left = false;
   bool _move_right = false;
   bool _move_forward = false;
   bool _move_backward = false;
};