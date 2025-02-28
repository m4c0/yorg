#version 450

layout(set = 0, binding = 0) uniform sampler2D terrain;

layout(location = 0) in vec2 f_pos;
layout(location = 1) in flat int f_inst;
layout(location = 2) in vec2 f_uv;

layout(location = 0) out vec4 colour;
layout(location = 1) out vec4 select;

void main() {
  colour = texture(terrain, f_uv);

  uint ir = f_inst % 256;
  uint ig = f_inst / 256;
  select = vec4(ir, ig, 0, 256) / 256.0;
}
