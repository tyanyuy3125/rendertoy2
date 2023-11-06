#pragma once

#include <memory>
#include <span>
#include <map>
#include <vector>

#include "rendertoy_internal.h"
#include "sampler.h"

namespace rendertoy
{
    class Light
    {
    public:
        /// @brief 
        /// @param scene 
        /// @param intersect_info 
        /// @param pdf 
        /// @return Radiance contribution.
        virtual const glm::vec3 Sample_Ld(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, const bool consider_normal = true) const = 0;
        virtual const glm::vec3 Sample_Le(const glm::vec3 &last_origin, const IntersectInfo &intersect_info, float &pdf) const = 0;
        virtual const float Phi() const = 0;
    };

    class SurfaceLight : public Light
    {
    private:
        std::shared_ptr<Primitive> _surface_primitive;
    public: // TODO: 临时措施
        std::shared_ptr<Emissive> _material;
    public:
        SurfaceLight() = delete;
        SurfaceLight(std::shared_ptr<Primitive> surface_primitive, std::shared_ptr<Emissive> material) : _surface_primitive(surface_primitive), _material(material) {}
        virtual const glm::vec3 Sample_Ld(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, const bool consider_normal = true) const;
        virtual const glm::vec3 Sample_Le(const glm::vec3 &last_origin, const IntersectInfo &intersect_info, float &pdf) const;

        virtual const float Phi() const;
    };

    class LightSampler
    {
    private:
        // std::map<std::shared_ptr<Light>, size_t> light_to_index;
        AliasTable alias_table;
    public:
        LightSampler(const std::vector<std::shared_ptr<Light> > &dls_lights);

        const int Sample(float *pmf) const
        {
            if(!alias_table.size())
            {
                return 0;
            }
            int ret = alias_table.Sample(glm::linearRand<float>(0.0f, 1.0f), pmf);
            return ret;
        }
    };
}