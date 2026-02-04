#include "scene.h"
#include <glad/glad.h>

void scene::pre_render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void scene::render()
{
}