#include "intersectinfo.h"

const glm::mat3 rendertoy::IntersectInfo::GenerateSurfaceCoordinates() const
{
    /*
    i   y   o
     \  |  /
      \ | /
        .---x
         \
          \
           z
    */
    glm::vec3 y = _normal;
    glm::vec3 z = glm::normalize(glm::cross(y, _in));
    if(std::abs(glm::dot(z,z)) < 1e-4f)
    {
        auto z_0 = glm::cross(y, glm::vec3(1.0f, 0.0f, 0.0f));
        auto z_1 = glm::cross(y, glm::vec3(0.0f, 1.0f, 0.0f));
        if(std::abs(glm::dot(z_0, z_0)) < 1e-4f)
        {
            z = glm::normalize(z_1);
        }
        else
        {
            z = glm::normalize(z_0);
        }
    }
    glm::vec3 x = glm::cross(y, z);
    return glm::mat3(x, y, z);
}