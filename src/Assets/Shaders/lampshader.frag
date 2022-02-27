#version 330 core
uniform vec3 colorAttr;
out vec4 color;

void main() { color = vec4(colorAttr, 1.0); }