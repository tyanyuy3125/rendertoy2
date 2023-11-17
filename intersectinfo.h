#pragma once

#include <memory>

#include "rendertoy_internal.h"

namespace rendertoy
{
    struct IntersectInfo
    {
        glm::vec3 _wo;
        glm::vec2 _uv;
        glm::vec3 _coord;
        glm::float32 _t;
        glm::vec3 _geometry_normal;
        glm::vec3 _shading_normal;
        std::shared_ptr<IMaterial> _mat;
        Primitive *_primitive;
        glm::float32 _time = 0.0f;

        bool _is_bsdf = true;
        const glm::mat3 GenerateSurfaceCoordinates() const;
    };

    struct VolumeInteraction
    {
        glm::vec3 _wo;
        glm::vec3 _coord;
        bool _valid = false;

        std::shared_ptr<PhaseFunction> _phase_func;
    };
}