#version 430 core

in vec2 texcoords;

out vec4 color;

uniform sampler2D screenTexture;

void main() {
    // color = vec4(floor(texcoords), 0.0, 1.0);
    color = texture(screenTexture, texcoords) * vec4(1.0, 0.0, 0.0, 1.0);
}