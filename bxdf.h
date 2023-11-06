#pragma once

#include "rendertoy_internal.h"

namespace rendertoy
{
    enum BxDFType
    {
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
        const float _eta;
        int nBxDFs = 0;
        static const int MaxBxDFs = 8;
        std::shared_ptr<BxDF> bxdfs[MaxBxDFs];

    public:
        BSDF() = delete;
        BSDF(const IntersectInfo &intersect_info, float eta = 1.0f);
        static bool IsSpecular(const BxDFType bxdf_type)
        {
            return bxdf_type & BSDF_SPECULAR;
        }
        void Add(const std::shared_ptr<BxDF> &b)
        {
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

    // BxDF Declarations
    class BxDF
    {
    public:
        // BxDF Interface
        virtual ~BxDF() {}
        BxDF(BxDFType type) : type(type) {}
        bool MatchesFlags(BxDFType t) const { return (type & t) == type; }
        virtual glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const = 0;
        virtual glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                                   float *pdf,
                                   BxDFType *sampledType = nullptr) const;
        // virtual glm::vec3 rho(const glm::vec3 &wo, int nSamples,
        //                       const glm::vec2 *samples) const;
        // virtual glm::vec3 rho(int nSamples, const glm::vec2 *samples1,
        //                       const glm::vec2 *samples2) const;
        virtual float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const;

        // BxDF Public Data
        const BxDFType type;
    };

    class LambertianReflection : public BxDF
    {
    public:
        // LambertianReflection Public Methods
        LambertianReflection(const glm::vec3 &R)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;
        // glm::vec3 rho(const glm::vec3 &, int, const glm::vec2 *) const { return R; }
        // glm::vec3 rho(int, const glm::vec2 *, const glm::vec2 *) const { return R; }

    private:
        // LambertianReflection Private Data
        const glm::vec3 R;
    };

    class OrenNayar : public BxDF
    {
    public:
        // OrenNayar Public Methods
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
        // OrenNayar Private Data
        const glm::vec3 R;
        float A, B;
    };

    class SpecularReflection : public BxDF
    {
    public:
        // SpecularReflection Public Methods
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
        // SpecularReflection Private Data
        const glm::vec3 R;
        const std::shared_ptr<Fresnel> fresnel;
    };

    class MicrofacetReflection : public BxDF
    {
    public:
        // MicrofacetReflection Public Methods
        MicrofacetReflection(const glm::vec3 &R,
                             std::shared_ptr<MicrofacetDistribution> distribution, std::shared_ptr<Fresnel> fresnel)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
              R(R),
              distribution(distribution),
              fresnel(fresnel) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;
        glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi, const glm::vec2 &u,
                           float *pdf, BxDFType *sampledType) const;
        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        // MicrofacetReflection Private Data
        const glm::vec3 R;
        const std::shared_ptr<MicrofacetDistribution> distribution;
        const std::shared_ptr<Fresnel> fresnel;
    };
}