#include <string>

static const std::string simple_vert_shader = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\0";

static const std::string simple_frag_shader = "#version 330 core\n"
    "out vec4 color;\n"
    "uniform vec4 vertex_color;\n"
    "void main()\n"
    "{\n"
    "   color = vertex_color;\n"
    "}\n\0";