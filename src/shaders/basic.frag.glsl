#version 410 core

// basic shader the using per-vertex colors with no lighting

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 color;

void main(){
    color = vec4(fragColor, 1.0);
}