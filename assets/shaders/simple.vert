#version 330 core
layout(location = 0) in vec3 a_Position;

uniform mat4 u_Model;

layout(std140) uniform Camera {
  mat4 u_View;
  mat4 u_Projection;
  vec4 u_CameraPos;
};

void main() {
  gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}
