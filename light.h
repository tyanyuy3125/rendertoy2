#pragma once

#include <memory>

#include "rendertoy_internal.h"
#include "primitive.h"

namespace rendertoy
{
    class SurfaceLight
    {
    public: // TODO: Standarize
        std::shared_ptr<Primitive> _surface_primitive;

        const glm::vec3 Sample(float &pdf) const;
    };
}