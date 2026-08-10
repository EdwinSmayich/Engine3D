#include "Camera.h"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(const glm::vec3& InPos, const glm::vec3& InUp, GLfloat InPitch, GLfloat InYaw)
    : Front(0.0f, 0.0f, -1.0f),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM)
{
    Position = InPos;
    WorldUp = InUp;
    Pitch = InPitch;
    Yaw = InYaw;
    UpdateCameraVectors();
}

Camera::Camera(GLfloat InPosX, GLfloat InPosY, GLfloat InPosZ, // Position
               GLfloat InUpX, GLfloat InUpY, GLfloat InUpZ,    // Up
               GLfloat InPitch, GLfloat InYaw)                 // Euler angles
    : Front(0.0f, 0.0, -1.0f),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM)
{
    Position = glm::vec3(InPosX, InPosY, InPosZ);
    WorldUp = glm::vec3(InUpX, InUpY, InUpZ);
    Pitch = InPitch;
    Yaw = InYaw;
    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    const GLfloat PitchRad = glm::radians(Pitch);
    const GLfloat YawRad = glm::radians(Yaw);

    glm::vec3 FrontDirection;
    FrontDirection.x = glm::cos(YawRad) * glm::cos(PitchRad);
    FrontDirection.y = glm::sin(PitchRad);
    FrontDirection.z = glm::sin(YawRad) * glm::cos(PitchRad);

    Front = glm::normalize(FrontDirection);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::ProcessKeyboard(CameraMovementType InDirection, GLfloat InDeltaTime)
{
    const GLfloat Velocity = MovementSpeed * InDeltaTime;
    switch (InDirection)
    {
        case CameraMovementType::CMT_Forward:
        {
            Position += Front * Velocity;
            break;
        }
        case CameraMovementType::CMT_Backward:
        {
            Position -= Front * Velocity;
            break;
        }
        case CameraMovementType::CMT_Up:
        {
            Position += Up * Velocity;
            break;
        }
        case CameraMovementType::CMT_Down:
        {
            Position -= Up * Velocity;
            break;
        }
        case CameraMovementType::CMT_Right:
        {
            Position += Right * Velocity;
            break;
        }
        case CameraMovementType::CMT_Left:
        {
            Position -= Right * Velocity;
            break;
        }
    }
}

void Camera::ProcessMouseMovement(GLfloat InOffsetX, GLfloat InOffsetY, GLboolean NewConstrainPitch)
{
    InOffsetX *= MouseSensitivity;
    InOffsetY *= MouseSensitivity;

    Yaw += InOffsetX;
    Pitch += InOffsetY;

    if (NewConstrainPitch)
    {
        if (Pitch > 89.0f)
        {
            Pitch = 89.0f;
        }
        if (Pitch < -89.0f)
        {
            Pitch = -89.0f;
        }
    }

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat InOffsetY)
{
    Zoom -= InOffsetY * MovementSpeed * MouseSensitivity;
    if (Zoom < 1.0f)
    {
        Zoom = 1.0f;
    }
    if (Zoom > 90.0f)
    {
        Zoom = 90.0f;
    }
}

void Camera::SetPosition(const glm::vec3& NewPos)
{
    Position = NewPos;
}

void Camera::SetSpeed(GLfloat NewSpeed)
{
    MovementSpeed = NewSpeed;
}

void Camera::SetSensitivity(GLfloat NewSens)
{
    MouseSensitivity = NewSens;
}

void Camera::SetFOV(GLfloat NewFOV)
{
    Zoom = NewFOV;
}

void Camera::SetPitch(GLfloat NewPitch)
{
    Pitch = NewPitch;
    UpdateCameraVectors();
}

void Camera::SetYaw(GLfloat NewYaw)
{
    Yaw = NewYaw;
    UpdateCameraVectors();
}

void Camera::ResetToDefaults()
{
    *this = Camera{};
}

glm::mat4 Camera::GetViewMatrix() const
{
    glm::mat4 Rotation(1.0f);
    Rotation[0][0] = Right.x;
    Rotation[1][0] = Right.y;
    Rotation[2][0] = Right.z;

    Rotation[0][1] = Up.x;
    Rotation[1][1] = Up.y;
    Rotation[2][1] = Up.z;

    Rotation[0][2] = -Front.x;
    Rotation[1][2] = -Front.y;
    Rotation[2][2] = -Front.z;

    glm::mat4 Translation(1.0f);
    Translation[3][0] = -Position.x;
    Translation[3][1] = -Position.y;
    Translation[3][2] = -Position.z;

    return Rotation * Translation;
}