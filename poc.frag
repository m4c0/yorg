#version 450

layout(set = 0, binding = 0) uniform sampler2D terrain;

layout(location = 0) in vec2 f_pos;
layout(location = 1) in flat int f_inst;
layout(location = 2) in vec2 f_uv;

layout(location = 0) out vec4 colour;
layout(location = 1) out vec4 select;

void main() {
  colour = texture(terrain, f_uv);
  select = vec4(f_inst / 256.0, 0, 0, 1);
}
