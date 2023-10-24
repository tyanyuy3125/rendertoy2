#pragma once

#include <glm/glm.hpp>

#include "rendertoy.h"

namespace rendertoy
{
    class BBox
    {
    private:
        glm::vec3 _pmin;
        glm::vec3 _pmax;
    public:
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction) const;
        const glm::vec3 GetCenter() const;
    };


}