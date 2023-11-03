#pragma once

#include <memory>
#include <vector>

#include "rendertoy_internal.h"
#include "accelerate.h"
#include "primitive.h"

namespace rendertoy
{
    class Scene
    {
    private:
        BVH<Primitive> _objects;
        std::vector<std::shared_ptr<Light>> _dls_lights;
        std::vector<std::shared_ptr<Light>> _lights;
        std::shared_ptr<LightSampler> _light_sampler;

        MATERIAL_SOCKET(hdr_background, Color);

    public:
        Scene() = default;
        Scene(const Scene &) = delete;

        const std::vector<std::shared_ptr<Primitive>> &objects() const
        {
            return _objects.objects;
        }
        std::vector<std::shared_ptr<Primitive>> &objects()
        {
            return _objects.objects;
        }

        void Init();
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const;
        const glm::vec3 SampleLights(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, SurfaceLight *&sampled_light) const;
    };
}
