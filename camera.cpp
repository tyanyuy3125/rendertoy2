#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

rendertoy::Camera::Camera(const glm::vec3 &origin, const glm::mat3 &rotation, const glm::float32 fov, const glm::float32 aspect_ratio)
    : _origin(origin), _rotation(rotation), _fov(fov), _aspect_ratio(aspect_ratio)
{
}

void rendertoy::Camera::SpawnRay(const glm::vec2 &coord, glm::vec3 RENDERTOY_FUNC_ARGUMENT_OUT origin, glm::vec3 RENDERTOY_FUNC_ARGUMENT_OUT direction) const
{
    glm::vec2 ndc = 2.0f * coord - glm::vec2(1.0f);
    glm::vec3 ray_direction = glm::normalize(glm::vec3(ndc.x * _aspect_ratio, ndc.y, -1.0f / std::tan(_fov / 2.0f)));

    ray_direction = _rotation * ray_direction;

    origin = _origin;
    direction = ray_direction;
}