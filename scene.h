#pragma once

#include "accelerate.h"
#include "primitive.h"
#include "accelerate.h"

namespace rendertoy
{
    class Scene
    {
    private:
        BVH<Primitive> _objects;
    public:
        Scene() = default;
        Scene(const Scene &) = delete;

        const std::vector<std::unique_ptr<Primitive>> &objects() const
        {
            return _objects.objects;
        }
        std::vector<std::unique_ptr<Primitive>> &objects()
        {
            return _objects.objects;
        }

        void Init();
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const;
    };
}
