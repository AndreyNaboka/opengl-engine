#include "game.h"
#include <chrono>
#include <thread>
#include <glfw/glfw3.h>

game::game()
{
   _start_frame_time = std::chrono::high_resolution_clock::now();
   _end_frame_time = _start_frame_time;
}

void game::begin_update()
{
   _start_frame_time = std::chrono::high_resolution_clock::now();
}

void game::end_update()
{
   _end_frame_time = std::chrono::high_resolution_clock::now();
   _delta_time = std::chrono::duration<float>(_end_frame_time - _start_frame_time).count();
   update_fps();
   if (_delta_time < get_target_fps_time())
   {
      std::this_thread::sleep_for(std::chrono::duration<float>(get_target_fps_time() - _delta_time));
   }
}

void game::update_fps()
{
   _number_of_frames++;
   _fps_timer += _delta_time;
   if (_fps_timer >= 1.0)
   {
      _fps = static_cast<int>(_number_of_frames / _fps_timer);
      _number_of_frames = 0;
      _fps_timer = 0.0;
   }
}
