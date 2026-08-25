#include "Transform.h"

void FTransform::UpdateRotationFromEuler()
{
    glm::quat QYaw = glm::angleAxis(glm::radians(RotationEuler.y), glm::vec3(0.0, 1.0f, 0.0f));
    glm::quat QPitch = glm::angleAxis(glm::radians(RotationEuler.x), glm::vec3(1.0, 0.0f, 0.0f));
    glm::quat QRoll = glm::angleAxis(glm::radians(RotationEuler.z), glm::vec3(0.0, 0.0f, 1.0f));

    Rotation = QYaw * QPitch * QRoll;
}

glm::mat4 FTransform::Matrix() const
{
    glm::mat4 T = glm::translate(glm::mat4(1.0f), Position);
    glm::mat4 R = glm::mat4_cast(Rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), Scale);

    return T * R * S;
}