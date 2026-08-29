#include "Transform.h"

void FTransform::UpdateRotationFromEuler()
{
    glm::quat QYaw = glm::angleAxis(glm::radians(RotationEuler.y), glm::vec3(0.0, 1.0f, 0.0f));
    glm::quat QPitch = glm::angleAxis(glm::radians(RotationEuler.x), glm::vec3(1.0, 0.0f, 0.0f));
    glm::quat QRoll = glm::angleAxis(glm::radians(RotationEuler.z), glm::vec3(0.0, 0.0f, 1.0f));

    Rotation = QYaw * QPitch * QRoll;
}

void FTransform::SetPitch(GLfloat NewPitch)
{
    RotationEuler.x = NewPitch;
    UpdateRotationFromEuler();
}

void FTransform::SetYaw(GLfloat NewYaw)
{
    RotationEuler.y = NewYaw;
    UpdateRotationFromEuler();
}

void FTransform::SetRoll(GLfloat NewRoll)
{
    RotationEuler.z = NewRoll;
    UpdateRotationFromEuler();
}

glm::mat4 FTransform::GetMatrix() const
{
    glm::mat4 T = glm::translate(glm::mat4(1.0f), Position);
    glm::mat4 R = glm::mat4_cast(Rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), Scale);

    return T * R * S;
}