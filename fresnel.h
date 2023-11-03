#pragma once

#include "rendertoy_internal.h"

// Borrowed from PBRT-v3

namespace rendertoy
{
    class Fresnel
    {
    public:
        virtual ~Fresnel() {};
        virtual glm::vec3 Evaluate(float cosI) const = 0;
    };

    class FresnelConductor : public Fresnel
    {
    public:
        // FresnelConductor Public Methods
        glm::vec3 Evaluate(float cosThetaI) const;
        FresnelConductor(const glm::vec3 &etaI, const glm::vec3 &etaT,
                         const glm::vec3 &k)
            : etaI(etaI), etaT(etaT), k(k) {}

    private:
        glm::vec3 etaI, etaT, k;
    };

    class FresnelDielectric : public Fresnel
    {
    public:
        // FresnelDielectric Public Methods
        glm::vec3 Evaluate(float cosThetaI) const;
        FresnelDielectric(float etaI, float etaT) : etaI(etaI), etaT(etaT) {}

    private:
        float etaI, etaT;
    };

    class FresnelNoOp : public Fresnel
    {
    public:
        glm::vec3 Evaluate(float) const { return glm::vec3(1.); }
    };
}
