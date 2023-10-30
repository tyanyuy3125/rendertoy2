#pragma once

#include <glm/glm.hpp>

#include "rendertoy_internal.h"

namespace rendertoy
{
    class Camera
    {
    private:
        glm::vec3 _origin;
        glm::mat3 _rotation;
        glm::float32 _fov;
    
    public:
        void SpawnRay(const glm::vec2 &coord, glm::vec3 RENDERTOY_FUNC_ARGUMENT_OUT origin, glm::vec3 RENDERTOY_FUNC_ARGUMENT_OUT direction) const;
    };
}