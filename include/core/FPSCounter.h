#pragma once
#include <deque>
#include <chrono>

class FPSCounter
{
public:
    void Tick()
    {
        auto now = std::chrono::high_resolution_clock::now();
        _timestamps.push_back(now);
        // удаляем все старше 1 секунды
        while (_timestamps.size() >= 2)
        {
            auto ageUs = std::chrono::duration_cast<std::chrono::microseconds>(now - _timestamps.front()).count();
            if (ageUs > 1'000'000)
                _timestamps.pop_front();
            else
                break;
        }
    }

    float GetFPS() const
    {
        if (_timestamps.size() < 2)
            return 0.0f;
        auto timespanUs = std::chrono::duration_cast<std::chrono::microseconds>(_timestamps.back() - _timestamps.front()).count();
        if (timespanUs == 0)
            return 0.0f;
        return (static_cast<float>(_timestamps.size() - 1) * 1'000'000.0f) / static_cast<float>(timespanUs);
    }

private:
    std::deque<std::chrono::high_resolution_clock::time_point> _timestamps;
};