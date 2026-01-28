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
}

void game::update()
{
   auto now = std::chrono::high_resolution_clock::now();
   _frame_times.push_back(now);
   while (_frame_times.size() > 1)
   {
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _frame_times.front()).count();
      if (duration >= 1)
      {
         _frame_times.pop_front();
      }
      else
      {
         break;
      }
   }
}

const double game::get_fps()
{
   if (_frame_times.size() <= 1)
      return 0.0f;
   auto duration = std::chrono::duration_cast<std::chrono::microseconds>(_frame_times.back() - _frame_times.front()).count();
   if (duration == 0)
      return 0.0f;
   return static_cast<float>(_frame_times.size() - 1) * 1'000'000.0f / static_cast<float>(duration);
}
