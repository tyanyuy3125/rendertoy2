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
        std::vector<std::shared_ptr<Light>> _inf_lights;
        std::shared_ptr<LightSampler> _light_sampler;
        

        MATERIAL_SOCKET(hdr_background, Color);

    public:
        std::shared_ptr<Medium> _global_medium; // TODO: Temporary

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

        const std::vector<std::shared_ptr<Light>> &lights() const
        {
            return _lights;
        }
        std::vector<std::shared_ptr<Light>> &lights()
        {
            return _lights;
        }

        const std::vector<std::shared_ptr<Light>> &inf_lights() const
        {
            return _inf_lights;
        }
        std::vector<std::shared_ptr<Light>> &inf_lights()
        {
            return _inf_lights;
        }

        void Init();
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo &intersect_info) const;
        const bool Intersect(const glm::vec3 &p0, const glm::vec3 &p1) const;
        const glm::vec3 SampleLights(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, const bool consider_normal, bool &do_heuristic) const;
        const glm::vec3 SampleLights(const VolumeInteraction &v_i, float &pdf, glm::vec3 &direction, bool &do_heuristic) const;
        const Light *SampleLights(float *pmf) const;
    };
}
