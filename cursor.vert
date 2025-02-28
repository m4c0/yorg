#version 450

layout(push_constant) uniform upc {
  vec2 anchor;
  vec2 window_size;
};

layout(location = 0) in vec2 pos;

layout(location = 0) out vec2 f_pos;

void main() {
  vec2 p = pos - 0.5;
  p = (anchor + p * 10) / window_size;
  p = p * 2.0 - 1.0;

  gl_Position = vec4(p, 0, 1);
  f_pos = pos;
}
