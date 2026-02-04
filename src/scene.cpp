#include <glad/glad.h>

#include "scene.h"
#include "render_object.h"

scene::scene(const std::string &name)
{
    _name = name;
    _render_objects_list.reserve(100);
}

void scene::pre_render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void scene::render()
{
    for (auto obj : _render_objects_list)
    {
        obj->render();
    }
}