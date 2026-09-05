#pragma once
#include "glad/gl.h"
#include <cstdint>
#include "Light.h"
#include "../Math/Transform.h"

enum class EObjectType : std::uint8_t
{
    EOT_Light,
    EOT_Cube
};

struct USceneObject
{
    EObjectType ObjectType = EObjectType::EOT_Light;
    FTransform Transform;
    GLfloat BoundingRadius = 0.4f;
    FLight LightData;
};
