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
        virtual const glm::vec3 Sample_Ld(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction) const = 0;
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
        virtual const glm::vec3 Sample_Ld(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction) const;

        virtual const float Phi() const;
    };

    class LightSampler
    {
    private:
        std::map<Light, size_t> light_to_index;
        AliasTable alias_table;
    public:
        // float PMF
        LightSampler(const std::vector<std::shared_ptr<Light> > &dls_lights);
    };
}