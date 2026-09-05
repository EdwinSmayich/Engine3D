#include "Math.h"
#include "glm/detail/func_geometric.inl"

namespace Math
{
    // clang-format off
    bool RayHitsSphere(glm::vec3 InO, glm::vec3 InD, glm::vec3 InC, GLfloat InR, GLfloat& OutT)
    {
        glm::vec3 OC = InC - InO;                       // from the camera to the center of the sphere
        float Tca = glm::dot(OC, InD);              // projection of OC onto the ray (where the closest approach occurs)
        if (Tca < 0.0f)
        {
            return false;                               // sphere BEHIND the ray — misses
        }

        float D2 = glm::dot(OC, OC) - Tca * Tca;    // square of the distance from the center to the ray line
        float R2 = InR * InR;
        if (D2 > R2)
        {
            return false;                               // the ray passes BY the sphere
        }

        float Thc = glm::sqrt(R2 - D2);              // half of the chord inside the sphere
        OutT = Tca - Thc;                               // distance to the NEAREST entry point
        return true;
    }
    
    bool RayHitsPlane(glm::vec3 InO, glm::vec3 InD, glm::vec3 InPlanePoint, glm::vec3 InPlaneNormal, glm::vec3& OutHit)
    {
        float Denom = glm::dot(InD, InPlaneNormal);
        if (glm::abs(Denom) < 1e-6f)
        {
            return false;                               // If a ray is parallel to a plane, it will not intersect it.
        }
        
        float T = glm::dot(InPlanePoint - InO, InPlaneNormal) / Denom;
        if (T < 0.0f)
        {
            return false;                               // the plane behind the camera
        }
        
        OutHit = InO + T * InD;                         // point of impact
        return true;
    }
    // clang-format on
} // namespace Math