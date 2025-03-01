#version 450

layout(set = 0, binding = 0) uniform sampler2D terrain;

layout(location = 0) in vec2 f_pos;
layout(location = 1) in flat int f_inst;
layout(location = 2) in vec2 f_uv;
layout(location = 3) in float f_pickable;

layout(location = 0) out vec4 colour;
layout(location = 1) out uvec4 select;

void main() {
  colour = texture(terrain, f_uv);

  uint i1 = f_inst % 256;
  uint i2 = f_inst / 256;
  select = uvec4(i1, i2, 0, f_pickable);
}
