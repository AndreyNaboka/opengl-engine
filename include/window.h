#pragma once
#include <string>
#include <memory>

class window
{
public:
    static std::shared_ptr<window> create(const std::string& title, const int w, const int h);
private:
    window(const std::string& title, const int w, const int h);
};