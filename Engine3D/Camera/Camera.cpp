#include "Camera.h"

Camera::Camera(const glm::vec3& InPos)
    : MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Fov(ZOOM)
{
    Transform.Position = InPos;
    Transform.UpdateRotationFromEuler();
}

void Camera::ProcessKeyboard(ECameraMovementType InDirection, GLfloat InDeltaTime)
{
    glm::vec3 CameraPos = Transform.Position;
    const GLfloat Velocity = MovementSpeed * InDeltaTime;

    switch (InDirection)
    {
        case ECameraMovementType::ECMT_Forward:
        {
            CameraPos += Transform.GetFrontVector() * Velocity;
            break;
        }
        case ECameraMovementType::ECMT_Backward:
        {
            CameraPos -= Transform.GetFrontVector() * Velocity;
            break;
        }
        case ECameraMovementType::ECMT_Up:
        {
            CameraPos += Transform.GetUpVector() * Velocity;
            break;
        }
        case ECameraMovementType::ECMT_Down:
        {
            CameraPos -= Transform.GetUpVector() * Velocity;
            break;
        }
        case ECameraMovementType::ECMT_Right:
        {
            CameraPos += Transform.GetRightVector() * Velocity;
            break;
        }
        case ECameraMovementType::ECMT_Left:
        {
            CameraPos -= Transform.GetRightVector() * Velocity;
            break;
        }
    }

    Transform.Position = CameraPos;
}

void Camera::ProcessMouseMovement(GLfloat InOffsetX, GLfloat InOffsetY, GLboolean NewConstrainPitch)
{
    InOffsetX *= MouseSensitivity;
    InOffsetY *= MouseSensitivity;

    Transform.RotationEuler.y -= InOffsetX;
    Transform.RotationEuler.x += InOffsetY;

    if (NewConstrainPitch)
    {
        Transform.RotationEuler.x = glm::min(Transform.RotationEuler.x, 89.0f);
        Transform.RotationEuler.x = glm::max(Transform.RotationEuler.x, -89.0f);
    }

    Transform.UpdateRotationFromEuler();
}

void Camera::ProcessMouseScroll(GLfloat InOffsetY)
{
    Fov -= InOffsetY * MovementSpeed * MouseSensitivity;
    Fov = glm::max(Fov, 1.0f);
    Fov = glm::min(Fov, 90.0f);
}

void Camera::ResetToDefaults()
{
    *this = Camera{};
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::inverse(Transform.GetMatrix());
}