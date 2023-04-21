#version 430 core

in vec2 texcoords;

out vec4 color;

void main() {
    color = vec4(texcoords, 0.0, 1.0);
}