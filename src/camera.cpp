#include "camera.h"
#include "settings.h"

#include "glad/glad.h"

camera::camera()
{
   _pos   = glm::vec3(0.0f, 0.0f, 3.0f);
   _front = glm::vec3(0.0f, 0.0f, -1.0f);
   _up    = glm::vec3(0.0f, 1.0f, 0.0f);

   _proj_matrix = glm::perspective(45.0f, WINDOW_ASPECT_RATIO, 0.1f, 100.0f);
}

void camera::move_left()
{
}

void camera::move_right()
{
}

void camera::move_forward()
{
}

void camera::move_backward()
{
}
