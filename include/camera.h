#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class camera
{
public:
   camera();
   void set_speed(const float new_speed) { _speed = new_speed; }
   void set_mouse_sensitivity(const float mouse_sensitivity) { _mouse_sensitivity = mouse_sensitivity; }
   void move_left();
   void move_right();
   void move_forward();
   void move_backward();
   void mouse_move(const float x, const float y, bool constrain_pitch = true);
   void mouse_scroll(const float x_offset, const float y_offset);

   const glm::mat4& get_proj_matrix() const { return _proj_matrix; }
   const glm::mat4& get_view_matrix();

private:
   void update_camera_vectors();

private:
   float _yaw   = -90.0f;
   float _pitch =  0.0f;
   float _zoom  =  45.0f;  
   float _speed =  3.0f;
   float _mouse_sensitivity = 0.25f;

   bool _need_update_matrix = false;

   glm::mat4 _view_matrix;
   glm::mat4 _proj_matrix;
   glm::vec3 _pos;
   glm::vec3 _front;
   glm::vec3 _up;
   glm::vec3 _world_up;
};