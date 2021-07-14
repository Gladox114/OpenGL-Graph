#version 330 core
layout (location = 0) in vec3 coord2d;
out vec4 f_color;

void main(void) {
  gl_Position = vec4(coord2d,1.0f);
  f_color = vec4(1.0f,0.5f,1.0f,1.0f);
}

