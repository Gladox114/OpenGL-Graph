#version 330 core
layout (location = 0) in vec2 coord2d;
//attribute vec2 coord2d;
//varying vec4 f_color;
out vec4 f_color;
//uniform float offset_x;
//uniform float scale_x;
uniform vec4 color;
void main(void) {
  gl_Position = vec4(coord2d, 0.0,1);
  //f_color = vec4(coord2d.xy / 2.0 + 0.5, 1, 1);
  f_color = vec4(color);
}

