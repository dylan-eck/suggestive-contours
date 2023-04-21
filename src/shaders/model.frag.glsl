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

    // lighting calculations are performed in view space
    // so that it is easy to find the direction to the viewer
    vec3 viewFragPos = (viewMatrix * mMat * vec4(fragPos, 1.0)).xyz;
    vec3 viewFragNormal = normalize((viewMatrix * mMat * vec4(fragNormal, 0.0)).xyz);

    vec3 viewLightPos = (viewMatrix * vec4(lightPosition, 1.0)).xyz;
    vec3 dirToLight = viewLightPos - viewFragPos;
    float lightDist = length(dirToLight);
    dirToLight = normalize(dirToLight);

    vec3 viewDir = normalize(-viewFragPos);
    vec3 diffuseIntensity = diffuseReflectivity * max(dot(dirToLight, viewFragNormal), 0.0);

    // combine light components
    // use lighting from spot light, unless it is less than the ambient light
    vec3 light = max(diffuseIntensity, ambientReflectivity * ambientIntensity);

    color = vec4(vec3(max(dot(viewDir, viewFragNormal), 0.0)), 1.0);
}