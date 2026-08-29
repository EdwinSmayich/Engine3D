#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

struct FTransform
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 RotationEuler = glm::vec3(0.0f);              // What a person controls (degrees)
    glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // What the engine calculates
    glm::vec3 Scale = glm::vec3(1.0f);

    void UpdateRotationFromEuler();
    glm::mat4 GetMatrix() const;
};
