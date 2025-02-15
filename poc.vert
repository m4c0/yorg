#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 i_pos;

layout(location = 0) out vec2 f_pos;

void main() {
  vec2 p = pos + i_pos;
  p -= 8.0;
  p = p / 8.0;

  gl_Position = vec4(p, 0, 1);
  f_pos = pos;
}
