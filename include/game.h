#pragma once

#include <chrono>

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
   void end_update();

   const double get_delta_time() { return _delta_time; };
   const double get_fps() { return _fps; };
   const float get_target_fps_time() const { 1.0f / _target_fps; }
   const bool is_need_update_frame() const { return _need_update_frame; }

private:
   game();
   ~game() = default;
   void update_fps();

private:
   std::chrono::high_resolution_clock::time_point _end_frame_time;
   std::chrono::high_resolution_clock::time_point _start_frame_time;
   int _fps = 0;
   int _number_of_frames = 0;
   bool _need_update_frame = false;
   float _delta_time = 0.0f;
   float _fps_timer = 0.0f;
   static const unsigned int _target_fps = 120;
};
