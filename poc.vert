#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 i_pos;

layout(location = 0) out vec2 f_pos;
layout(location = 1) out ivec2 f_inst;

void main() {
  uint id = gl_InstanceIndex;
  ivec2 pp = ivec2(id % 16, id / 16);

  vec2 p = pos + pp;
  p -= 8.0;
  p = p / 8.0;

  gl_Position = vec4(p, 0, 1);
  f_pos = pos;
  f_inst = pp;
}
