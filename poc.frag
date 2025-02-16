#version 450

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;
layout(location = 1) out vec4 select;

void main() {
  colour = vec4(f_pos, 0, 1);
  select = vec4(f_pos, 0, 1);
}
