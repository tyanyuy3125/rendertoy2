#pragma once

#include "rendertoy_internal.h"

namespace rendertoy
{
    class MicrofacetDistribution
    {
    public:
        virtual ~MicrofacetDistribution() {}
        virtual const float D(const glm::vec3 &wh) const = 0;
        virtual const float Lambda(const glm::vec3 &w) const = 0;
        const float G1(const glm::vec3 &w) const
        {
            return 1 / (1 + Lambda(w));
        }
        virtual const float G(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return 1 / (1 + Lambda(wo) + Lambda(wi));
        }
        virtual const glm::vec3 Sample_wh(const glm::vec3 &wo, const glm::vec2 &u) const = 0;
        const float Pdf(const glm::vec3 &wo, const glm::vec3 &wh) const
        {
            if (sampleVisibleArea)
                return D(wh) * G1(wo) * AbsDot(wo, wh) / AbsCosTheta(wo);
            else
                return D(wh) * AbsCosTheta(wh);
        }

    protected:
        MicrofacetDistribution(bool sampleVisibleArea)
            : sampleVisibleArea(sampleVisibleArea) {}

        const bool sampleVisibleArea;
    };

    class BeckmannDistribution : public MicrofacetDistribution
    {
    public:
        static const float RoughnessToAlpha(float roughness)
        {
            roughness = std::max(roughness, 1e-3f);
            float x = std::log(roughness);
            return 1.62142f + 0.819955f * x + 0.1734f * x * x +
                   0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
        }
        BeckmannDistribution(float alphax, float alphay, bool samplevis = true)
            : MicrofacetDistribution(samplevis),
              alphax(std::max(0.001f, alphax)),
              alphay(std::max(0.001f, alphay)) {}
        virtual const float D(const glm::vec3 &wh) const;
        virtual const glm::vec3 Sample_wh(const glm::vec3 &wo, const glm::vec2 &u) const;

    private:
        virtual const float Lambda(const glm::vec3 &w) const;
        const float alphax, alphay;
    };
}