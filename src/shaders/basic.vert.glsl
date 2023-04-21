#version 430 core

// basic shader the using per-vertex colors with no lighting

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

layout(location = 0) out vec3 fragColor;

uniform mat4 mMat;

layout(std140, binding = 0) uniform globalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

void main(){
    fragColor = vColor;
    gl_Position = projectionMatrix * viewMatrix * mMat * vec4(vPosition, 1.0);
}