#version 430 core

in vec2 texcoords;

out vec4 fragColor;

uniform sampler2D screenTexture;

const float r = 4.0;
const float s = 0.3;
const float d = 0.05;

float intensity(vec4 color) {
    return dot(color.rgb, vec3(0.299, 0.587, 0.114));
}

bool isValley(vec2 texcoord, float radius, float s, float d) {
    float p_i = intensity(texture(screenTexture, texcoord));
    float p_max = 0;
    int numDarker = 0;
    int total = 0;

    vec2 pixelSize = 1.0 / vec2(textureSize(screenTexture, 0));
    vec2 radiusPixels = radius * pixelSize;

    for (float x = -radiusPixels.x; x <= radiusPixels.x; x += pixelSize.x) {
        for (float y = -radiusPixels.y; y <= radiusPixels.y; y += pixelSize.x) {
            if ((x * x + y * y) > (radiusPixels.x * radiusPixels.y)) continue;

            vec2 offset = vec2(x, y);
            vec2 sampleCoord = texcoord + offset;
            float sampleIntensity = intensity(texture(screenTexture, sampleCoord));

            p_max = max(p_max, sampleIntensity);
            numDarker += int(sampleIntensity < p_i);
            total += 1;
        }
    }

    float scaled_s = (1.0 - 1.0 / radius) * s;
    float scaled_d = radius * d;

    return (float(numDarker) / float(total) <= scaled_s) && (p_max - p_i >= scaled_d);
}

void main() {
    if (isValley(texcoords, r, s, d)) {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        fragColor = vec4(1.0);
    }
}