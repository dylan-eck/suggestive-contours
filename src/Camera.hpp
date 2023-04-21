#ifndef CAMERA_HPP
#define CAMERA_HPP

// this class is used for managing a camera
// and view and projection matrices

#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::mat4 getViewMatrix();

    void setProjection(
        float verticalFieldOfViewDegrees,
        float aspectRatio,
        float nearClipDistance,
        std::optional<float> farClipDistance
    );

    void moveArcBall(double deltaX, double deltaY, int width, int height);

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 upVector;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

#endif