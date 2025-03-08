#version 450

layout(location = 0) in flat uvec4 f_inst;
layout(location = 0) out uvec4 select;

void main() {
  select = f_inst;
}
