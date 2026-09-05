#pragma once
#include "../Math/Transform.h"
#include "glad/gl.h"
#include <glm/glm.hpp>

enum class ECameraMovementType : glm::uint8_t
{
    ECMT_Forward,
    ECMT_Backward,
    ECMT_Up,
    ECMT_Down,
    ECMT_Right,
    ECMT_Left
};

constexpr GLfloat SPEED = 15.0f;
constexpr GLfloat SENSITIVITY = 0.1f;
constexpr GLfloat ZOOM = 45.0f;

// clang-format off
class ACamera
{
public:
    ACamera(const glm::vec3& InPos = glm::vec3(0.0f, 0.0f, 20.0f));
    
    void ProcessKeyboard(ECameraMovementType InDirection, GLfloat InDeltaTime);
    void ProcessMouseMovement(GLfloat InOffsetX, GLfloat InOffsetY, GLboolean NewConstrainPitch = true);
    void ProcessMouseScroll(GLfloat InOffsetY);
    
    void SetSpeed       (GLfloat NewSpeed) { MovementSpeed = NewSpeed; }
    void SetSensitivity (GLfloat NewSens)  { MouseSensitivity = NewSens; }
    void SetFOV         (GLfloat NewFOV)   { Fov = NewFOV; }
    void ResetToDefaults();
    
    GLfloat   GetSpeed()       const { return MovementSpeed; }
    GLfloat   GetSensitivity() const { return MouseSensitivity; }
    GLfloat   GetFOV()         const { return Fov; }
    glm::mat4 GetViewMatrix()  const;

    FTransform&       GetTransform()       { return Transform; }
    const FTransform& GetTransform() const { return Transform; }
    
private:
    // Camera attributes
    FTransform Transform;

    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Fov;
};
// clang-format on