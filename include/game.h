#pragma once

#include <chrono>
#include <deque>

class game
{
public:
   game(const game &) = delete;
   game &operator=(const game &) = delete;
   static game &instance()
   {
      static game g;
      return g;
   }
   void begin_update();
   void update();
   void end_update();

   const double get_delta_time() { return _delta_time; };
   const double get_fps();

private:
   game();
   ~game() = default;

private:
   std::chrono::high_resolution_clock::time_point _end_frame_time;
   std::chrono::high_resolution_clock::time_point _start_frame_time;
   float _delta_time = 0.0f;
   static constexpr float _target_fps_time = 1.0f / 120.0f;
   std::deque<std::chrono::high_resolution_clock::time_point> _frame_times;
};
