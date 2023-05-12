#version 430 core

in vec2 texcoords;

out vec4 fragColor;

uniform sampler2D screenTexture;

const float radius = 4.0;

const int MAX_WINDOW_RADIUS = 20;
int samples[MAX_WINDOW_RADIUS * MAX_WINDOW_RADIUS];

void main() {
    fragColor = texture(screenTexture, texcoords);
}