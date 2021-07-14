#version 330 core
layout (location = 0) in vec2 coord2d;
//attribute vec2 coord2d;
//varying vec4 f_color;
out vec4 f_color;
uniform float offset_x;
uniform float scale_x;
uniform float wheel_scale_x;
uniform float offset_y;
uniform float scale_y;

uniform vec3 color;
void main(void) {
  //gl_Position = vec4((coord2d.x+offset_x) * scale_x, (coord2d.y+offset_y) * scale_y, 0.0,1.0);
  gl_Position = vec4((coord2d.x* scale_x + offset_x)*wheel_scale_x, coord2d.y * scale_y + offset_y, 0.0,1.0);
  //f_color = vec4(coord2d.xy / 2.0 + 0.5, 1, 1);
  /*f_color = vec4( (coord2d.y/2.0+0.7)*color.r, 
                  (coord2d.y/2.0+0.7)*color.g, 
                  (coord2d.y/2.0+0.7)*color.b, 
                  1);*/
  f_color = vec4(color,1);
}

