#version 450

layout(location = 0) in flat uint f_inst;
layout(location = 0) out uint select;

void main() {
  select = f_inst;
}
