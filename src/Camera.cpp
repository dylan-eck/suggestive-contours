#include "Camera.hpp"

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, target, upVector);
}

// sets the camera's projection matrix
// far clippling plane distance is optional, if not passed an infinite
// clipping is assumed
void Camera::setProjection(
    float verticalFieldOfViewDegrees,
    float aspectRatio,
    float nearClipDistance,
    std::optional<float> farClipDistance
) {
    float fov = glm::radians(verticalFieldOfViewDegrees);
    if (farClipDistance.has_value()) {
        projectionMatrix = glm::perspective(
            fov,
            aspectRatio,
            nearClipDistance,
            farClipDistance.value()
        );
    } else {
        float n = 0.1f;
        float e = 1.0f / glm::tan(fov / 2.0f);
        float a = aspectRatio;
        float eps = 2.4e-7;

        projectionMatrix = glm::mat4(
            glm::vec4(e, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, e / a, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, eps - 1.0f, (eps - 2.0f) * n),
            glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)
        );

        projectionMatrix = glm::transpose(projectionMatrix);
    }
}

void Camera::moveArcBall(double deltaX, double deltaY, int width, int height) {
    float dthetax = deltaX * (glm::two_pi<double>() / width);
    float dthetay = deltaY * (glm::two_pi<double>() / height);

    glm::vec4 viewDir = -glm::transpose(viewMatrix)[2];
    glm::vec3 rightVec = glm::transpose(viewMatrix)[0];

    float cosAngle = glm::dot(viewDir, glm::vec4(upVector, 1.0f));
    glm::mat4 xRot = glm::mat4(1.0f);
    xRot = glm::rotate(xRot, (float)dthetax, glm::vec3(0, 1, 0));
    position = (xRot * (glm::vec4(position, 1.0f) - glm::vec4(target, 1.0f))) + glm::vec4(target, 1.0f);

    glm::mat4 yRot = glm::mat4(1.0f);
    yRot = glm::rotate(yRot, (float)dthetay, rightVec);
    position = (yRot * (glm::vec4(position, 1.0f) - glm::vec4(target, 1.0f))) + glm::vec4(target, 1.0f);

    if (position.y < 0.1) position.y = 0.1;
}