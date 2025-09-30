#include <string>

static const std::string simple_vert_shader = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\0";

static const std::string simple_frag_shader = "#version 330 core\n"
    "out vec4 color;\n"
    "uniform float u_time;\n"
    "void main()\n"
    "{\n"
    "   float progress = fract(u_time * 0.5);\n"
    "   vec3 vertex_color = mix(vec3(0.0, 1.0, 0.0),\n"
    "                      vec3(0.0, 0.0, 1.0),\n"
    "                      progress);\n"
    "   color = vec4(vertex_color, 1.0f);\n"
    "}\n\0";

static const std::string textured_vert_shader = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 2) in vec2 texCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model\n;"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(position, 1.0f);\n"
    "   TexCoord = texCoord;\n"
    "}\0";
static const std::string textured_frag_shader = "#version 330 core\n"
    "out vec4 color;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "   color = texture(ourTexture, TexCoord);\n"
    "}\n\0";