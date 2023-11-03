#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "rendertoy_internal.h"

namespace rendertoy
{
    struct IntersectInfo
    {
        glm::vec3 _in;
        glm::vec2 _uv;
        glm::vec3 _coord;
        glm::float32 _t;
        glm::vec3 _normal;
        std::shared_ptr<IMaterial> _mat;
        Primitive *_primitive;

        const glm::mat3 GenerateSurfaceCoordinates() const;
    };
}