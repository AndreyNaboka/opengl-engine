#pragma once

class game {
public:
   game();
   void update();
   const double get_delta_time() { return _delta_time; };
   const double get_fps() { return _fps; };
   
private:
   void update_fps();

private:
   double _prev_time = 0.0;
   double _current_time = 0.0;
   double _delta_time = 0.0;
   int _number_of_frames = 0;
   double _fps = 0.0;
   double _fps_timer = 0.0;
};
