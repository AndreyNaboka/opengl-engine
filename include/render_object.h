#pragma once

#include <glad/glad.h>
#include <string>

class shader;
class texture;

class render_object
{
public:
    render_object(const std::string &name) { _name = name; }
    void init();
    void render();

private:
    std::string _name;
    GLuint _vao = 0;
    GLuint _vbo = 0;
    
};