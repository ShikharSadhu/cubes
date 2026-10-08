#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float YAW = -90.0f;
const float PITCH = -35.0f;
const float DISTANCE = 5.0f;
const float ORB_SENSITIVITY = 0.3f;
const float PAN_SENSITIVITY = 0.01;

class Camera {
public:

    glm::vec3 Target;
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    float Yaw;
    float Pitch;
    float Distance;

    Camera(
        glm::vec3 target = glm::vec3(0.0f),
        float distance = DISTANCE,
        float yaw = YAW,
        float pitch = PITCH
    )
        : Target(target),
        Yaw(yaw),
        Pitch(pitch),
        Distance(distance)
    {
    }

    glm::mat4 GetViewMatrix()
    {
        glm::vec3 position = GetPosition();

        return glm::lookAt(
            position,
            Target,
            Up
        );
    }

    void Orbit(float xOffset, float yOffset) {
        Yaw += xOffset * ORB_SENSITIVITY;
        Pitch += yOffset * ORB_SENSITIVITY;

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    void Pan(float xOffset, float yOffset) {
        glm::vec3 position = GetPosition();
        glm::vec3 forward = glm::normalize(Target - position);
        glm::vec3 right = glm::normalize(glm::cross(forward, Up));
        glm::vec3 up = glm::normalize(glm::cross(right, forward));

        Target -= right * xOffset * PAN_SENSITIVITY;
        Target -= up * yOffset * PAN_SENSITIVITY;
    }

    void Zoom(float amount) {
        Distance -= amount;

        if (Distance < 1.0f)
            Distance = 1.0f;
        if (Distance > 100.f)
            Distance = 100.f;
    }

private:

    glm::vec3 GetPosition() const
    {
        glm::vec3 direction;

        direction.x = glm::cos(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
        direction.y = glm::sin(glm::radians(Pitch));
        direction.z = glm::sin(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));

        direction = glm::normalize(direction);

        return Target - direction * Distance;
    }
};

#endif