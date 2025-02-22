#version 450

layout(push_constant) uniform upc { float aspect; };

layout(location = 0) in vec2 pos;
layout(location = 1) in uint i_id;

layout(location = 0) out vec2 f_pos;
layout(location = 1) out uint f_inst;
layout(location = 2) out uint f_id;

void main() {
  uint id = gl_InstanceIndex;
  ivec2 pp = ivec2(id % 16, id / 16);

  vec2 p = pos + pp;
  p -= 8.0;
  p = p / 8.0;

  p *= 0.9;
  p.x /= aspect;

  gl_Position = vec4(p, 0, 1);
  f_pos = pos;
  f_inst = id;
  f_id = i_id;
}
