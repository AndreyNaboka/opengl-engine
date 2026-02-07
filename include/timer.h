#pragma once
#include <chrono>

class timer
{
public:
    float mark()
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - _last_time;
        _last_time = now;
        return elapsed.count();
    }

private:
    std::chrono::high_resolution_clock::time_point _last_time = std::chrono::high_resolution_clock::now();
};