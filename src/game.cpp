#include "game.h"

#include <glfw/glfw3.h>

game::game()
{
   _current_time = glfwGetTime();
   _prev_time = _current_time;
}

void game::update()
{
   _current_time = glfwGetTime();
   _delta_time = _current_time - _prev_time;

   update_fps();

   _prev_time = _current_time;
}

void game::update_fps()
{
   _number_of_frames++;
   _fps_timer += _delta_time;
   if (_fps_timer > 1.0)
   {
      _fps = static_cast<double>(_number_of_frames / _fps_timer);
      _number_of_frames = 0;
      _fps_timer = 0.0;
   }
}
