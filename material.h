#pragma once

#include <memory>
#include <algorithm>

#include "rendertoy_internal.h"
#include "logger.h"

namespace rendertoy
{
    class IMaterial
    {
        MATERIAL_SOCKET(albedo, Color);

    public:
        IMaterial(const std::shared_ptr<ISamplableColor> &albedo)
            : _albedo(albedo) {}
        /// @brief
        /// @param uv
        /// @note Plane light should not be IES-like, so taking uv is enough.
        /// @return
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const
        {
            return glm::vec3(0.0f);
        }

        virtual const std::unique_ptr<BSDF> GetBSDF(const IntersectInfo &intersect_info) const = 0;
    };

    class DiffuseBSDF : public IMaterial
    {
        MATERIAL_SOCKET(sigma, Numerical)

    public:
        DiffuseBSDF(const std::shared_ptr<ISamplableColor> &albedo, const std::shared_ptr<ISamplableNumerical> &sigma)
            : IMaterial(albedo), _sigma(sigma)
        {}

        virtual const std::unique_ptr<BSDF> GetBSDF(const IntersectInfo &intersect_info) const;
    };

    class Emissive : public IMaterial
    {
        MATERIAL_SOCKET(strength, Numerical)
    public:
        Emissive(const std::shared_ptr<ISamplableColor> &albedo, const std::shared_ptr<ISamplableNumerical> &strength)
            : IMaterial(albedo), _strength(strength) {}
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const;

        virtual const std::unique_ptr<BSDF> GetBSDF(const IntersectInfo &intersect_info) const;
    };

    class SpecularBSDF : public IMaterial
    {
    public:
        SpecularBSDF(const std::shared_ptr<ISamplableColor> &albedo)
            : IMaterial(albedo) {}
        
        virtual const std::unique_ptr<BSDF> GetBSDF(const IntersectInfo &intersect_info) const;
    };

    class MetalBSDF : public IMaterial
    {
        MATERIAL_SOCKET(eta, Color)
        MATERIAL_SOCKET(k, Color)
        MATERIAL_SOCKET(u_roughness, Numerical)
        MATERIAL_SOCKET(v_roughness, Numerical)
        bool _remap_roughness;
    public:
        MetalBSDF(const std::shared_ptr<ISamplableColor> &albedo,
                  const std::shared_ptr<ISamplableColor> &eta,
                  const std::shared_ptr<ISamplableColor> &k,
                  const std::shared_ptr<ISamplableNumerical> &u_roughness,
                  const std::shared_ptr<ISamplableNumerical> &v_roughness,
                  const bool remap_roughness)
            : IMaterial(albedo),
              _eta(eta),
              _k(k),
              _u_roughness(u_roughness),
              _v_roughness(v_roughness),
              _remap_roughness(remap_roughness)
        {
        }

        virtual const std::unique_ptr<BSDF> GetBSDF(const IntersectInfo &intersect_info) const;
    };
}