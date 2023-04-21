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

vec3 blinnPhongSpecular(vec3 dirToLight, vec3 viewNormal, vec3 viewDir) {
    vec3 halfway = normalize(dirToLight + viewDir);
    vec3 specularIntensity = specularReflectivity * pow(max(dot(halfway, viewNormal), 0.0), shininess);
    return specularIntensity;
}

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
    vec3 specularIntensity = blinnPhongSpecular(dirToLight, viewFragNormal, viewDir);

    // make light intensity decrease with distance from the light source
    float attenuation = clamp(1.0 - (lightDist * lightDist) / (attenuationRadius * attenuationRadius), 0.0, 1.0);
    attenuation *= attenuation;

    // combine light components
    // use lighting from spot light, unless it is less than the ambient light
    vec3 light = max(attenuation * (diffuseIntensity + specularIntensity), ambientReflectivity * ambientIntensity);

    color = vec4(light, 1.0);
}