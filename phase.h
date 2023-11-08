#pragma once

#include "rendertoy_internal.h"

namespace rendertoy
{
    class PhaseFunction
    {
    public:
        virtual const float p(const glm::vec3 &wo, const glm::vec3 &wi) const = 0;
        virtual const glm::vec3 Sample_p(const glm::vec3 &wo, float *p) const = 0;
    };

    class IsotropicPhaseFunction : public PhaseFunction
    {
    public:
        virtual const float p(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return 0.25f * glm::one_over_pi<float>();
        }
        virtual const glm::vec3 Sample_p(const glm::vec3 &wo, float *p);
    };
}