#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec2 vUV;

out vertData {
    vec3 position;
    vec3 color;
    vec3 normal;
} vertDataOut;

uniform mat4 mMat;

layout(std140, binding = 0) uniform globalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewportMatrix;
};

void main() {
    vertDataOut.position = vPosition;
    vertDataOut.color = vColor;
    vertDataOut.normal = vNormal;

    gl_Position = projectionMatrix * viewMatrix * mMat * vec4(vPosition, 1.0);
}