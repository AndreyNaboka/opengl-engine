#include "window.h"

std::shared_ptr<window> window::create(const std::string& title, const int w, const int h)
{
    int b = 0;
    return std::shared_ptr<window>(new window(title, w, h));
}

window::window(const std::string& title, const int w, const int h) {}