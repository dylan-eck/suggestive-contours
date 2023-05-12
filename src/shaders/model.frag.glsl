#version 430 core

// fragment shader for per-fragment lighting

in vertData {
    vec3 position;
    vec3 color;
    vec3 normal;
} vertDataIn;

layout(location = 0) out vec4 color;

uniform mat4 mMat;

layout(std140, binding = 0) uniform globalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewportMatrix;
};

layout(std140, binding = 1) uniform materialUBO {
    vec3 ambientReflectivity;
    vec3 diffuseReflectivity;
    vec3 specularReflectivity;
    float shininess;
};

layout(std140, binding = 2) uniform lightingUBO {
    vec3 lightPosition;
    vec3 spotLightDirection;
    vec3 ambientIntensity;
    float attenuationRadius;
    float innerCutoff;
    float outerCutoff;
};

void main(){
    vec3 fragPos = vertDataIn.position;
    vec3 fragNormal = vertDataIn.normal;

    vec3 viewFragPos = (viewMatrix * mMat * vec4(fragPos, 1.0)).xyz;
    vec3 viewFragNormal = normalize((viewMatrix * mMat * vec4(fragNormal, 0.0)).xyz);
    vec3 viewDir = normalize(-viewFragPos);

    color = vec4(vec3(max(dot(viewDir, viewFragNormal), 0.0)), 1.0);
}