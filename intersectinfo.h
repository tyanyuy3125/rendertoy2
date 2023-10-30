#pragma once

#include <glm/glm.hpp>

#include "rendertoy_internal.h"

namespace rendertoy
{
    struct IntersectInfo
    {
        bool _intersected;
        glm::vec2 _uv;
        glm::vec3 _coord;
        glm::float32 _t;
        glm::vec3 _normal;
    };
}