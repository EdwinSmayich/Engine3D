#pragma once
#include <glad/gl.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

struct FTransform
{
    // clang-format off
    void UpdateRotationFromEuler();
    
    void SetPitch (GLfloat NewPitch);
    void SetYaw   (GLfloat NewYaw);
    void SetRoll  (GLfloat NewRoll);
    
    GLfloat   GetPitch()         const { return RotationEuler.x; }
    GLfloat   GetYaw()           const { return RotationEuler.y; }
    GLfloat   GetRoll()          const { return RotationEuler.z; }
    glm::vec3 GetFrontVector()   const { return Rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
    glm::vec3 GetUpVector()      const { return Rotation * glm::vec3(0.0f, 1.0f, 0.0f); }
    glm::vec3 GetRightVector()   const { return Rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
    glm::mat4 GetMatrix()        const;
    // clang-format on

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 RotationEuler = glm::vec3(0.0f);              // What a person controls (degrees)
    glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // What the engine calculates
    glm::vec3 Scale = glm::vec3(1.0f);
};
