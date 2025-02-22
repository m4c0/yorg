#version 450

layout(location = 0) in vec2 f_pos;
layout(location = 1) in flat int f_inst;
layout(location = 2) in flat uint f_id;

layout(location = 0) out vec4 colour;
layout(location = 1) out vec4 select;

void main() {
  colour = vec4(f_pos, f_id, 1);
  select = vec4(f_inst / 256.0, 0, 0, 1);
}
