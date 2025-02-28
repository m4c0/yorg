#version 450

layout(push_constant) uniform upc {
  vec2 anchor;
  vec2 window_size;
};

layout(location = 0) in vec2 pos;

layout(location = 0) out vec2 f_pos;

void main() {
  gl_Position = vec4(pos, 0, 1);
  f_pos = pos;
}
