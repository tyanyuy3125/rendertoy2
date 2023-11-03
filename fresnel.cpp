// Borrowed from PBRT-v3

#include "fresnel.h"

namespace rendertoy
{
    // BxDF Utility Functions
    float FrDielectric(float cosThetaI, float etaI, float etaT)
    {
        cosThetaI = glm::clamp(cosThetaI, -1.0f, 1.0f);
        // Potentially swap indices of refraction
        bool entering = cosThetaI > 0.0f;
        if (!entering)
        {
            std::swap(etaI, etaT);
            cosThetaI = std::abs(cosThetaI);
        }

        // Compute _cosThetaT_ using Snell's law
        float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
        float sinThetaT = etaI / etaT * sinThetaI;

        // Handle total internal reflection
        if (sinThetaT >= 1)
            return 1;
        float cosThetaT = std::sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));
        float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
                      ((etaT * cosThetaI) + (etaI * cosThetaT));
        float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
                      ((etaI * cosThetaI) + (etaT * cosThetaT));
        return (Rparl * Rparl + Rperp * Rperp) / 2;
    }

    // https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
    glm::vec3 FrConductor(float cosThetaI, const glm::vec3 &etai,
                         const glm::vec3 &etat, const glm::vec3 &k)
    {
        cosThetaI = glm::clamp(cosThetaI, -1.0f, 1.0f);
        glm::vec3 eta = etat / etai;
        glm::vec3 etak = k / etai;

        float cosThetaI2 = cosThetaI * cosThetaI;
        float sinThetaI2 = 1.f - cosThetaI2;
        glm::vec3 eta2 = eta * eta;
        glm::vec3 etak2 = etak * etak;

        glm::vec3 t0 = eta2 - etak2 - sinThetaI2;
        glm::vec3 a2plusb2 = glm::sqrt(t0 * t0 + 4.0f * eta2 * etak2);
        glm::vec3 t1 = a2plusb2 + cosThetaI2;
        glm::vec3 a = glm::sqrt(0.5f * (a2plusb2 + t0));
        glm::vec3 t2 = (float)2 * cosThetaI * a;
        glm::vec3 Rs = (t1 - t2) / (t1 + t2);

        glm::vec3 t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
        glm::vec3 t4 = t2 * sinThetaI2;
        glm::vec3 Rp = Rs * (t3 - t4) / (t3 + t4);

        return 0.5f * (Rp + Rs);
    }

    glm::vec3 FresnelConductor::Evaluate(float cosThetaI) const
    {
        return FrConductor(std::abs(cosThetaI), etaI, etaT, k);
    }

    glm::vec3 FresnelDielectric::Evaluate(float cosThetaI) const
    {
        return glm::vec3(FrDielectric(cosThetaI, etaI, etaT));
    }
}