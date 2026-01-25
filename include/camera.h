#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "input_system/input_system_subscriber.h"

class camera : public input_system_subscriber
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

   virtual void on_key(int code, int scancode, int action, int mods) override;
   virtual void on_mouse(double xpos, double ypos) override;
   virtual void on_scroll(double xoffset, double yoffset) override;

   const glm::mat4 &get_proj_matrix() const { return _proj_matrix; }
   const glm::mat4 &get_view_matrix();

private:
   void update_camera_vectors();
   void move_camera(const camera::camera_direction dir);

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
};