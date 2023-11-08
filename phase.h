#pragma once

#include "rendertoy_internal.h"

namespace rendertoy
{
    class PhaseFunction
    {
    public:
        PhaseFunction() = default;
        virtual const float p(const glm::vec3 &wo, const glm::vec3 &wi) const = 0;
        virtual const glm::vec3 Sample_p(const glm::vec3 &wo, float *p) const = 0;
    };

    class IsotropicPhaseFunction : public PhaseFunction
    {
    public:
        IsotropicPhaseFunction() = default;
        virtual const float p(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return 0.25f * glm::one_over_pi<float>();
        }
        virtual const glm::vec3 Sample_p(const glm::vec3 &wo, float *p) const;
    };

    class HenyeyGreensteinPhaseFunction : public PhaseFunction
    {
    public:
        HenyeyGreensteinPhaseFunction(const float g) : _g(g) {}
        virtual const float p(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return HenyeyGreenstein(glm::dot(wo, wi), _g);
        }
        virtual const glm::vec3 Sample_p(const glm::vec3 &wo, float *p) const
        {
            glm::vec3 wi = SampleHenyeyGreenstein(wo, _g, p);
            return wi;
        }

    private:
        static float HenyeyGreenstein(const float cosTheta, const float g)
        {
            float denom = 1.0f + (g * g) + 2.0f * g * cosTheta;
            return 0.25f * glm::one_over_pi<float>() * (1.0f - (g * g)) / (denom * std::sqrt(denom));
        }

        static glm::vec3 SampleHenyeyGreenstein(glm::vec3 wo, float g, float *pdf)
        {
            float rand = glm::linearRand(0.0f, 1.0f);
            float cosTheta;
            if (std::abs(g) < 1e-3)
                cosTheta = 1 - 2 * rand;
            else
            {
                float sqrTerm = (1 - g * g) / (1 + g - 2 * g * rand);
                cosTheta = -(1 + g * g - sqrTerm * sqrTerm) / (2 * g);
            }

            float sinTheta = std::sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta));
            float phi = 2 * glm::pi<float>() * glm::linearRand(0.0f, 1.0f);
            glm::vec3 v1, v2;
            CoordinateSystem(wo, &v1, &v2);
            *pdf = HenyeyGreenstein(cosTheta, g);
            return SphericalDirection(sinTheta, cosTheta, phi, v1, v2, wo);
        }
        float _g;
    };
}