#pragma once

#include <memory>
#include <algorithm>

#include "rendertoy_internal.h"
#include "logger.h"

namespace rendertoy
{
    // BxDFFlags Definition
    // Borrowed from pbrt-v4
    enum BxDFFlags
    {
        Unset = 0,
        Reflection = 1 << 0,
        Transmission = 1 << 1,
        Diffuse = 1 << 2,
        Glossy = 1 << 3,
        Specular = 1 << 4,
        // Composite _BxDFFlags_ definitions
        DiffuseReflection = Diffuse | Reflection,
        DiffuseTransmission = Diffuse | Transmission,
        GlossyReflection = Glossy | Reflection,
        GlossyTransmission = Glossy | Transmission,
        SpecularReflection = Specular | Reflection,
        SpecularTransmission = Specular | Transmission,
        All = Diffuse | Glossy | Specular | Reflection | Transmission
    };

    inline bool IsReflective(BxDFFlags f)
    {
        return f & BxDFFlags::Reflection;
    }
    inline bool IsTransmissive(BxDFFlags f)
    {
        return f & BxDFFlags::Transmission;
    }
    inline bool IsDiffuse(BxDFFlags f)
    {
        return f & BxDFFlags::Diffuse;
    }
    inline bool IsGlossy(BxDFFlags f)
    {
        return f & BxDFFlags::Glossy;
    }
    inline bool IsSpecular(BxDFFlags f)
    {
        return f & BxDFFlags::Specular;
    }
    inline bool IsNonSpecular(BxDFFlags f)
    {
        return f & (BxDFFlags::Diffuse | BxDFFlags::Glossy);
    }

    class IMaterial
    {
        MATERIAL_SOCKET(albedo, Color);

    private:
        virtual const glm::vec3 EvalLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv, float &pdf) const
        {
            CRIT << "Undefined" << std::endl;
            return glm::vec3(0.0f);
        }
        virtual const glm::vec3 SampleLocal(const glm::vec3 &wo, const glm::vec2 &uv, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const
        {
            CRIT << "Undefined" << std::endl;
            return glm::vec3(0.0f, 0.0f, 1.0f);
        }

    protected:
        virtual const float PdfLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv) const
        {
            return 1.0f / (2.0f * glm::pi<float>());
        }

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
        /// @brief
        /// @param intersect_info
        /// @param out
        /// @return BSDF.
        virtual const glm::vec3 Eval(const IntersectInfo &intersect_info, const glm::vec3 &wi, float &pdf) const;
        /// @brief Generates new sample light from the intersection point, and computes corresponding PDF & BSDF.
        /// @param intersect_info
        /// @param pdf
        /// @param bsdf
        /// @return Direction of new sample light.
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const;
    };

    class DiffuseBSDF : public IMaterial
    {
    private:
        bool _use_lambertian = true;
        float A, B;
        virtual const glm::vec3 EvalLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv, float &pdf) const;
        virtual const glm::vec3 SampleLocal(const glm::vec3 &wo, const glm::vec2 &uv, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const;

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
    };

    class Emissive : public IMaterial
    {
        MATERIAL_SOCKET(strength, Numerical);
        virtual const glm::vec3 EvalLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv, float &pdf) const;
        virtual const glm::vec3 SampleLocal(const glm::vec3 &wo, const glm::vec2 &uv, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const;

    public:
        Emissive(const std::shared_ptr<ISamplableColor> &albedo, const std::shared_ptr<ISamplableNumerical> &strength)
            : IMaterial(albedo), _strength(strength) {}
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const;
    };

    class SpecularBSDF : public IMaterial
    {
        virtual const glm::vec3 EvalLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv, float &pdf) const;
        virtual const glm::vec3 SampleLocal(const glm::vec3 &wo, const glm::vec2 &uv, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const;
        virtual const float PdfLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv) const;

        std::shared_ptr<Fresnel> _fresnel;

    public:
        SpecularBSDF(const std::shared_ptr<ISamplableColor> &albedo);
    };

}