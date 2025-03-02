#version 450

layout(location = 0) in flat int f_inst;

layout(location = 0) out uvec4 select;

void main() {
  uint i1 = f_inst % 256;
  uint i2 = f_inst / 256;
  select = uvec4(i1, i2, 0, 1);
}
