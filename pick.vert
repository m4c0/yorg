#version 450

layout(push_constant) uniform upc { float aspect; };

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 i_pos;

layout(location = 0) out uint f_inst;

void main() {
  uint id = gl_InstanceIndex;

  // TODO: move to a shared "something" with spr
  vec2 p = pos + i_pos;
  p -= 8.0;
  p = p / 8.0;

  p *= 0.9;
  p.x /= aspect;

  gl_Position = vec4(p, 0, 1);
  f_inst = gl_InstanceIndex;
}
