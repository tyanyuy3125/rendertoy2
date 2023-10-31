#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

rendertoy::Camera::Camera(const glm::vec3 &origin, const glm::mat3 &rotation, const glm::float32 fov, const glm::float32 aspect_ratio)
    : _origin(origin), _rotation(rotation), _fov(fov), _aspect_ratio(aspect_ratio)
{
}

rendertoy::Camera::Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up, const glm::float32 fov, const glm::float32 aspect_ratio)
    : _fov(fov), _aspect_ratio(aspect_ratio)
{
    this->LookAt(eye, center, up);
}

void rendertoy::Camera::SpawnRay(glm::vec2 coord, glm::vec3 RENDERTOY_FUNC_ARGUMENT_OUT origin, glm::vec3 RENDERTOY_FUNC_ARGUMENT_OUT direction) const
{
    coord.y = 1.0f - coord.y;
    glm::vec2 ndc = 2.0f * coord - glm::vec2(1.0f);
    glm::vec3 ray_direction = glm::normalize(glm::vec3(ndc.x * _aspect_ratio, ndc.y, -1.0f / std::tan(_fov / 2.0f)));

    ray_direction = _rotation * ray_direction;

    origin = _origin;
    direction = ray_direction;
}

void rendertoy::Camera::LookAt(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
{
    /*
        y   center
        | /
        |/
        o - - x
       /
      /
    z
    */
   glm::vec3 z_dir = eye - center;
   glm::vec3 x = glm::normalize(glm::cross(up, z_dir));
   glm::vec3 z = glm::normalize(z_dir);
   glm::vec3 y = glm::cross(z, x);
   _origin = eye;
   _rotation = glm::mat3(x, y, z);
}
