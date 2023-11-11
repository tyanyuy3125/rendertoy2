#pragma once

#include <memory>

#include "rendertoy_internal.h"

namespace rendertoy
{
    enum BxDFType
    {
        BSDF_NONE = 0,
        BSDF_REFLECTION = 1 << 0,
        BSDF_TRANSMISSION = 1 << 1,
        BSDF_DIFFUSE = 1 << 2,
        BSDF_GLOSSY = 1 << 3,
        BSDF_SPECULAR = 1 << 4,
        BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION |
                   BSDF_TRANSMISSION,
    };

    class BSDF
    {
        const glm::mat3 _ltw;
        int nBxDFs = 0;
        static const int MaxBxDFs = 8;
        std::shared_ptr<BxDF> bxdfs[MaxBxDFs];
        float weights_cdf[MaxBxDFs];

    public:
        const float _eta;
        BSDF() = delete;
        BSDF(const IntersectInfo &intersect_info, float eta = 1.0f);
        static bool IsSpecular(const BxDFType bxdf_type)
        {
            return bxdf_type & BSDF_SPECULAR;
        }
        bool IsTransmissive() const;
        void Add(const std::shared_ptr<BxDF> &b, const float weight = 1.0f)
        {
            weights_cdf[nBxDFs] = (nBxDFs == 0) ? (weight) : (weights_cdf[nBxDFs - 1] + weight);
            bxdfs[nBxDFs++] = b;
        }
        const glm::vec3 LocalToWorld(const glm::vec3 &w) const
        {
            return _ltw * w;
        }
        const glm::vec3 WorldToLocal(const glm::vec3 &w) const
        {
            return glm::transpose(_ltw) * w;
        }
        int NumComponents(BxDFType flags = BSDF_ALL) const;

        const glm::vec3 f(const glm::vec3 &wo_w, const glm::vec3 &wi_w,
                          BxDFType flags = BSDF_ALL) const;
        const glm::vec3 Sample_f(const glm::vec3 &wo_w, glm::vec3 *wi_w,
                                 float *pdf, BxDFType type = BSDF_ALL,
                                 BxDFType *sampled_type = nullptr) const;
        const float Pdf(const glm::vec3 &wo_w, const glm::vec3 &wi_w,
                        BxDFType flags = BSDF_ALL);
    };

    class BxDF
    {
    public:
        virtual ~BxDF() {}
        BxDF(BxDFType type) : type(type) {}
        bool MatchesFlags(BxDFType t) const { return (type & t) == type; }
        virtual glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const = 0;
        virtual glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                                   float *pdf, BxDFType *sampledType = nullptr) const;
        virtual float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const;

        const BxDFType type;
    };

    class LambertianReflection : public BxDF
    {
    public:
        LambertianReflection(const glm::vec3 &R)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        const glm::vec3 R;
    };

    class OrenNayar : public BxDF
    {
    public:
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;
        OrenNayar(const glm::vec3 &R, float sigma)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R)
        {
            sigma = glm::radians(sigma);
            float sigma2 = sigma * sigma;
            A = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
            B = 0.45f * sigma2 / (sigma2 + 0.09f);
        }

    private:
        const glm::vec3 R;
        float A, B;
    };

    class SpecularReflection : public BxDF
    {
    public:
        SpecularReflection(const glm::vec3 &R, std::shared_ptr<Fresnel> fresnel)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
              R(R),
              fresnel(fresnel) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return glm::vec3(0.f);
        }
        glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                           float *pdf, BxDFType *sampledType) const;
        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const { return 0.0f; }

    private:
        const glm::vec3 R;
        const std::shared_ptr<Fresnel> fresnel;
    };

    class MicrofacetReflection : public BxDF
    {
    public:
        MicrofacetReflection(const glm::vec3 &R,
                             std::shared_ptr<MicrofacetDistribution> distribution, std::shared_ptr<Fresnel> fresnel)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
              R(R),
              distribution(distribution),
              fresnel(fresnel) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;
        glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                           float *pdf, BxDFType *sampledType = nullptr) const;
        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        const glm::vec3 R;
        const std::shared_ptr<MicrofacetDistribution> distribution;
        const std::shared_ptr<Fresnel> fresnel;
    };

    class SpecularTransmission : public BxDF
    {
    public:
        SpecularTransmission(const glm::vec3 &T, float etaA, float etaB);
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return glm::vec3(0.f);
        }
        glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                           float *pdf, BxDFType *sampledType = nullptr) const;
        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const { return 0.0f; }

    private:
        const glm::vec3 T;
        const float etaA, etaB;
        const std::shared_ptr<FresnelDielectric> fresnel;
    };

    class MicrofacetTransmission : public BxDF
    {
    public:
        MicrofacetTransmission(const glm::vec3 &T,
                               std::shared_ptr<MicrofacetDistribution> distribution, float etaA,
                               float etaB);
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;
        glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                           float *pdf, BxDFType *sampledType) const;
        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        const glm::vec3 T;
        const std::shared_ptr<MicrofacetDistribution> distribution;
        const float etaA, etaB;
        const std::shared_ptr<FresnelDielectric> fresnel;
    };

    class FresnelSpecular : public BxDF
    {
    public:
        FresnelSpecular(const glm::vec3 &R, const glm::vec3 &T, float etaA,
                        float etaB)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)),
              R(R),
              T(T),
              etaA(etaA),
              etaB(etaB)
        {
        }
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return glm::vec3(0.f);
        }
        glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                           float *pdf, BxDFType *sampledType) const;
        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const { return 0; }

    private:
        const glm::vec3 R, T;
        const float etaA, etaB;
    };

    class LambertianTransmission : public BxDF
    {
    public:
        LambertianTransmission(const glm::vec3 &T)
            : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)), T(T) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return T * glm::one_over_pi<float>();
        }
        glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                          float *pdf, BxDFType *sampledType) const;
        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            return !SameHemisphere(wo, wi) ? AbsCosTheta(wi) * glm::one_over_pi<float>() : 0.0f;
        }

    private:
        glm::vec3 T;
    };
}