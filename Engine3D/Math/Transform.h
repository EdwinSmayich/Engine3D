#pragma once
#include "glm/ext/matrix_transform.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct FTransform
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);
    glm::mat4 Matrix() const
    {
        glm::mat4 M = glm::translate(glm::mat4(1.0f), Position);
        M = glm::scale(M, Scale);
        return M;
    }
};
