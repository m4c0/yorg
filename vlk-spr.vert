#version 450

layout(push_constant) uniform upc { float aspect; };

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 i_pos;
layout(location = 2) in vec2 i_uv;

layout(location = 0) out vec2 f_pos;
layout(location = 1) out vec2 f_uv;

void main() {
  uint id = gl_InstanceIndex;

  // TODO: move to a shared "something" with pick
  vec2 p = pos + i_pos;
  p -= 8.0;
  p = p / 8.0;

  p *= 0.9;
  p.x /= aspect;

  gl_Position = vec4(p, 0, 1);
  f_pos = pos;
  f_uv = (i_uv + pos) / 16;
}
