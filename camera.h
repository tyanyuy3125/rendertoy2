#pragma once

#include <optional>
#include <functional>

#include "rendertoy_internal.h"

namespace rendertoy
{
    class Camera
    {
    private:
        glm::vec3 _origin;
        glm::mat3 _rotation;
        glm::float32 _fov;
        glm::float32 _aspect_ratio;
        glm::float32 _lens_radius = 0.0f;
        glm::float32 _focal_distance = 4.0f;

    public:
        const glm::float32 &lens_radius() const
        {
            return _lens_radius;
        }
        glm::float32 &lens_radius()
        {
            return _lens_radius;
        }
        const glm::float32 &focal_distasnce() const
        {
            return _focal_distance;
        }
        glm::float32 &focal_distasnce()
        {
            return _focal_distance;
        }
        std::optional<std::function<bool(const glm::vec2 &)>> func_reject_lens_sampling = {};
        Camera() = delete;
        Camera(const glm::vec3 &origin, const glm::mat3 &rotation, const glm::float32 fov, const glm::float32 aspect_ratio);
        Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up, const glm::float32 fov, const glm::float32 aspect_ratio);
        void SpawnRay(glm::vec2 coord, glm::vec3 &origin, glm::vec3 &direction) const;
        void LookAt(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up);
    };
}