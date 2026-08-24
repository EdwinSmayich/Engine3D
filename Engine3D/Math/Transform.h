#pragma once
#include "glm/ext/matrix_transform.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct FTransform
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 RotationEuler = glm::vec3(0.0f); // X = Pitch, Y = Yaw, Z = Roll
    glm::vec3 Scale = glm::vec3(1.0f);
    glm::mat4 Matrix() const
    {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), Position);

        glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(RotationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Paw
        R = glm::rotate(R, glm::radians(RotationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));                         // Pitch
        R = glm::rotate(R, glm::radians(RotationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));                         // Roll

        glm::mat4 S = glm::scale(glm::mat4(1.0f), Scale);

        return T * R * S;
    }
};
