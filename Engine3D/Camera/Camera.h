#pragma once
#include "glad/gl.h"
#include <glm/glm.hpp>

enum class CameraMovementType
{
    CMT_Forward,
    CMT_Backward,
    CMT_Right,
    CMT_Left
};

constexpr GLfloat YAW = -90.0f;
constexpr GLfloat PITCH = 0.0f;
constexpr GLfloat SPEED = 10.0f;
constexpr GLfloat SENSITIVITY = 0.3f;
constexpr GLfloat ZOOM = 90.0f;

class Camera
{
public:
    Camera(const glm::vec3& InPos = glm::vec3(0.0f, 0.0f, 0.0f), // Position
           const glm::vec3& InUp = glm::vec3(0.0f, 1.0f, 0.0f),  // Up
           GLfloat InPitch = PITCH, GLfloat InYaw = YAW);        // Euler angles

    Camera(GLfloat InPosX, GLfloat InPosY, GLfloat InPosZ, // Position
           GLfloat InUpX, GLfloat InUpY, GLfloat InUpZ,    // Up
           GLfloat InPitch, GLfloat InYaw);                // Euler angles

private:
    void UpdateCameraVectors();

public:
    void ProcessKeyboard(CameraMovementType InDirection, GLfloat InDeltaTime);
    void ProcessMouseMovement(GLfloat InOffsetX, GLfloat InOffsetY, GLboolean NewConstrainPitch = true);
    void ProcessMouseScroll(GLfloat InOffsetY);

    GLfloat GetZoom() const;
    glm::mat4 GetViewMatrix() const;

private:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Up;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler angles
    GLfloat Pitch;
    GLfloat Yaw;

    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;
};
