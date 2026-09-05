#pragma once
#include "glad/gl.h"
#include "glm/vec3.hpp"

namespace Math
{
    /*
     * OC — The vector from the camera to the center of the sphere.
     * Tca — The distance along the ray from the point closest to the center. Negative → sphere behind → misses.
     * D2 - is the square of the perpendicular distance from the center of the sphere to the ray.
     * If R² is greater, the ray passed on the outside → it missed.
     * Thc — the half-length of the ray segment inside the sphere; Tca − Thc = the distance to the point where the ray enters the sphere.
     */
    bool RayHitsSphere(glm::vec3 InO, glm::vec3 InD, glm::vec3 InC, GLfloat InR, GLfloat& OutT);
    bool RayHitsPlane(glm::vec3 InO, glm::vec3 InD, glm::vec3 InPlanePoint, glm::vec3 InPlaneNormal, glm::vec3& OutHit);
}; // namespace Math
