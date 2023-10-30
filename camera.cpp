#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

void rendertoy::Camera::SpawnRay(const glm::vec2 &coord, glm::vec3 RENDERTOY_FUNC_ARGUMENT_OUT origin, glm::vec3 RENDERTOY_FUNC_ARGUMENT_OUT direction) const
{
    glm::vec2 ndc = 2.0f * coord - glm::vec2(1.0f);
    glm::vec3 ray_direction = glm::normalize(glm::vec3(ndc.x, ndc.y, -1.0f / tan(_fov / 2.0f)));
    
    ray_direction = _rotation * ray_direction;

    origin = _origin;
    direction = ray_direction;
}