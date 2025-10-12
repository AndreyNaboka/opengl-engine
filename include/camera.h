#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class camera
{
public:
   camera();
   void set_speed(const float new_speed) { _speed = new_speed; }
   void move_left();
   void move_right();
   void move_forward();
   void move_backward();
   const glm::mat4& get_proj_matrix() const { return _proj_matrix; }
   void mouse_move(const double x, const double y);
   void mouse_scroll(const double x_offset, const double y_offset);

private:
   float _speed = 1.0f;
   glm::mat4 _proj_matrix;
   glm::vec3 _pos;
   glm::vec3 _front;
   glm::vec3 _up;
};