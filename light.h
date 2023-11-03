#pragma once

#include <memory>

#include "rendertoy_internal.h"

namespace rendertoy
{
    class Scene;

    class Light
    {
    public:
        /// @brief 
        /// @param scene 
        /// @param intersect_info 
        /// @param pdf 
        /// @return Radiance contribution.
        virtual const glm::vec3 Sample(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction) const = 0;
    };

    class SurfaceLight : public Light
    {
    private:
        std::shared_ptr<Primitive> _surface_primitive;
    public: // TODO: 临时措施
        std::shared_ptr<IMaterial> _material;
    public:
        SurfaceLight() = delete;
        SurfaceLight(std::shared_ptr<Primitive> surface_primitive, std::shared_ptr<IMaterial> material) : _surface_primitive(surface_primitive), _material(material) {}
        virtual const glm::vec3 Sample(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction) const;
    };
}