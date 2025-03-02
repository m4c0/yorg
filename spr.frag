#version 450

layout(set = 0, binding = 0) uniform sampler2D terrain;

layout(location = 0) in vec2 f_pos;
layout(location = 1) in vec2 f_uv;

layout(location = 0) out vec4 colour;

void main() {
  colour = texture(terrain, f_uv);
}
