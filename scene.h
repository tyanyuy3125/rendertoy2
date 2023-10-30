#pragma once

#include "accelerate.h"
#include "primitive.h"

namespace rendertoy
{
    class Scene
    {
    public:
        BVH<Primitive> objects;

        void Init();
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const;
    };
}
