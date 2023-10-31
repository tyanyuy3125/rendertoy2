#pragma once

#include "accelerate.h"
#include "primitive.h"
#include "accelerate.h"
#include "texture.h"

namespace rendertoy
{
    static std::shared_ptr<ISamplable> default_hdr_background = std::make_shared<ColorTexture>(glm::vec4(0.1f));

    class Scene
    {
    private:
        BVH<Primitive> _objects;
        std::shared_ptr<ISamplable> _hdr_background = default_hdr_background;
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

        const std::shared_ptr<ISamplable> &hdr_background() const
        {
            return _hdr_background;
        }
        std::shared_ptr<ISamplable> &hdr_background()
        {
            return _hdr_background;
        }

        void Init();
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const;
    };
}
