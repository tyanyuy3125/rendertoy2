#pragma once

#include <memory>
#include <algorithm>

#include "rendertoy_internal.h"

namespace rendertoy
{
    class IMaterial
    {
        MATERIAL_SOCKET(albedo, Color);

    private:
        virtual const glm::vec3 EvalLocal(const glm::vec3 &wi, const glm::vec3 &wo, float &pdf) const = 0;

    public:
        IMaterial(const std::shared_ptr<ISamplableColor> &albedo)
            : _albedo(albedo) {}
        /// @brief
        /// @param uv
        /// @note Plane light should not be IES-like, so taking uv is enough.
        /// @return
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const = 0;
        /// @brief
        /// @param intersect_info
        /// @param out
        /// @return BSDF.
        virtual const glm::vec3 Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const;
        /// @brief Generates new sample light from the intersection point, and computes corresponding PDF & BSDF.
        /// @param intersect_info
        /// @param pdf
        /// @param bsdf
        /// @return Direction of new sample light.
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf) const = 0;
    };

    class DiffuseBSDF : public IMaterial
    {
    private:
        bool _use_lambertian = true;
        float A, B;
        virtual const glm::vec3 EvalLocal(const glm::vec3 &wi, const glm::vec3 &wo, float &pdf) const;

    public:
        DiffuseBSDF(const std::shared_ptr<ISamplableColor> &albedo, const float sigma = 0.0f)
            : IMaterial(albedo)
        {
            if (sigma == 0.0f)
            {
                _use_lambertian = true;
            }
            else
            {
                _use_lambertian = false;
                float sigma2 = sigma * sigma;
                A = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
                B = 0.45f * sigma2 / (sigma2 + 0.09f);
            }
        }
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const;
        // virtual const glm::vec3 Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const;
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf) const;
    };

    class Emissive : public IMaterial
    {
        MATERIAL_SOCKET(strength, Numerical);
        virtual const glm::vec3 EvalLocal(const glm::vec3 &wi, const glm::vec3 &wo, float &pdf) const;

    public:
        Emissive(const std::shared_ptr<ISamplableColor> &albedo, const std::shared_ptr<ISamplableNumerical> &strength)
            : IMaterial(albedo), _strength(strength) {}
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const;
        // virtual const glm::vec3 Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const;
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf) const;
    };

    class Specular : public IMaterial
    {
    public:
        Specular(const std::shared_ptr<ISamplableColor> &albedo)
        : IMaterial(albedo) {}
    };

}