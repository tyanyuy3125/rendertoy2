#include "intersectinfo.h"

const glm::mat3 rendertoy::IntersectInfo::GenerateSurfaceCoordinates() const
{
    /*
    i   z   o
     \  |  /
      \ | /
        .---x
         \
          \
           y
    */
    glm::vec3 z = _normal;
    glm::vec3 y = glm::normalize(glm::cross(z, _in));
    if(std::abs(glm::dot(y,y)) < 1e-4f)
    {
        auto y_0 = glm::cross(z, glm::vec3(1.0f, 0.0f, 0.0f));
        auto y_1 = glm::cross(z, glm::vec3(0.0f, 1.0f, 0.0f));
        if(std::abs(glm::dot(y_0, y_0)) < 1e-4f)
        {
            y = glm::normalize(y_1);
        }
        else
        {
            y = glm::normalize(y_0);
        }
    }
    glm::vec3 x = glm::cross(z, y);
    return glm::mat3(x, y, z);
}