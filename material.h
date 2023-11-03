#pragma once

#include <memory>
#include <algorithm>

#include "rendertoy_internal.h"

namespace rendertoy
{
    // BSDF Inline Functions (From PBRT)
    inline float CosTheta(const glm::vec3 &w) { return w.z; }
    inline float Cos2Theta(const glm::vec3 &w) { return w.z * w.z; }
    inline float AbsCosTheta(const glm::vec3 &w) { return std::abs(w.z); }
    inline float Sin2Theta(const glm::vec3 &w)
    {
        return std::max(0.0f, 1.0f - Cos2Theta(w));
    }
    inline float SinTheta(const glm::vec3 &w)
    {
        return std::sqrt(Sin2Theta(w));
    }
    inline float TanTheta(const glm::vec3 &w)
    {
        return SinTheta(w) / CosTheta(w);
    }
    inline float Tan2Theta(const glm::vec3 &w)
    {
        return Sin2Theta(w) / Cos2Theta(w);
    }
    inline float CosPhi(const glm::vec3 &w)
    {
        float sinTheta = SinTheta(w);
        return (sinTheta == 0) ? 1 : glm::clamp(w.x / sinTheta, -1.0f, 1.0f);
    }
    inline float SinPhi(const glm::vec3 &w)
    {
        float sinTheta = SinTheta(w);
        return (sinTheta == 0) ? 0 : glm::clamp(w.y / sinTheta, -1.0f, 1.0f);
    }
    inline float Cos2Phi(const glm::vec3 &w)
    {
        return CosPhi(w) * CosPhi(w);
    }
    inline float Sin2Phi(const glm::vec3 &w)
    {
        return SinPhi(w) * SinPhi(w);
    }
    inline float CosDPhi(const glm::vec3 &wa, const glm::vec3 &wb)
    {
        return glm::clamp((wa.x * wb.x + wa.y * wb.y) /
                              std::sqrt((wa.x * wa.x + wa.y * wa.y) *
                                        (wb.x * wb.x + wb.y * wb.y)),
                          -1.0f, 1.0f);
    }

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