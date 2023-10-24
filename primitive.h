#pragma once

#include <glm/glm.hpp>

#include "rendertoy.h"
#include "bbox.h"
#include "intersectinfo.h"

namespace rendertoy
{
    class Primitive
    {
    private:

    public:
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const = 0;
        virtual const BBox GetBoundingBox() const = 0;
    };

    class Sphere : public Primitive
    {
    private:
        glm::float32 radius;
    public:
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
    };
}