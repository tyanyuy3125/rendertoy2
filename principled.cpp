#include <numeric>

#include "principled.h"
#include "bxdf.h"
#include "fresnel.h"
#include "microfacet.h"
#include "texture.h"
#include "intersectinfo.h"

namespace rendertoy
{
    inline float sqr(float x) { return x * x; }

    inline float SchlickWeight(float cosTheta)
    {
        float m = glm::clamp(1.0f - cosTheta, 0.0f, 1.0f);
        return (m * m) * (m * m) * m;
    }

    inline float FrSchlick(float R0, float cosTheta)
    {
        return glm::mix(SchlickWeight(cosTheta), R0, 1.0f);
    }

    inline glm::vec3 FrSchlick(const glm::vec3 &R0, float cosTheta)
    {
        return glm::mix(glm::vec3(SchlickWeight(cosTheta)), R0, glm::vec3(1.0f));
    }

    inline float SchlickR0FromEta(float eta) { return sqr(eta - 1) / sqr(eta + 1); }

    class DisneyDiffuse : public BxDF
    {
    public:
        DisneyDiffuse(const glm::vec3 &R)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        glm::vec3 R;
    };

    glm::vec3 DisneyDiffuse::f(const glm::vec3 &wo, const glm::vec3 &wi) const
    {
        float Fo = SchlickWeight(AbsCosTheta(wo)),
              Fi = SchlickWeight(AbsCosTheta(wi));

        return R * glm::one_over_pi<float>() * (1 - Fo / 2) * (1 - Fi / 2);
    }

    // Borrowed from pbrt-v3
    class DisneyFakeSS : public BxDF
    {
    public:
        DisneyFakeSS(const glm::vec3 &R, float roughness)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
              R(R),
              roughness(roughness) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        glm::vec3 R;
        float roughness;
    };

    glm::vec3 DisneyFakeSS::f(const glm::vec3 &wo, const glm::vec3 &wi) const
    {
        glm::vec3 wh = wi + wo;
        if (wh.x == 0 && wh.y == 0 && wh.z == 0)
            return glm::vec3(0.);
        wh = glm::normalize(wh);
        float cosThetaD = glm::dot(wi, wh);

        // Fss90 used to "flatten" retroreflection based on roughness
        float Fss90 = cosThetaD * cosThetaD * roughness;
        float Fo = SchlickWeight(AbsCosTheta(wo)),
              Fi = SchlickWeight(AbsCosTheta(wi));
        float Fss = glm::mix(Fo, 1.0f, Fss90) * glm::mix(Fi, 1.0f, Fss90);
        // 1.25 scale is used to (roughly) preserve albedo
        float ss =
            1.25f * (Fss * (1.0f / (AbsCosTheta(wo) + AbsCosTheta(wi)) - .5f) + .5f);

        return R * glm::one_over_pi<float>() * ss;
    }

    // Borrowed from pbrt-v3
    class DisneyRetro : public BxDF
    {
    public:
        DisneyRetro(const glm::vec3 &R, float roughness)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
              R(R),
              roughness(roughness) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        glm::vec3 R;
        float roughness;
    };

    glm::vec3 DisneyRetro::f(const glm::vec3 &wo, const glm::vec3 &wi) const
    {
        glm::vec3 wh = wi + wo;
        if (wh.x == 0 && wh.y == 0 && wh.z == 0)
            return glm::vec3(0.);
        wh = glm::normalize(wh);
        float cosThetaD = glm::dot(wi, wh);

        float Fo = SchlickWeight(AbsCosTheta(wo)),
              Fi = SchlickWeight(AbsCosTheta(wi));
        float Rr = 2 * roughness * cosThetaD * cosThetaD;

        // Burley 2015, eq (4).
        return R * glm::one_over_pi<float>() * Rr * (Fo + Fi + Fo * Fi * (Rr - 1));
    }

    class DisneySheen : public BxDF
    {
    public:
        DisneySheen(const glm::vec3 &R)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        glm::vec3 R;
    };

    glm::vec3 DisneySheen::f(const glm::vec3 &wo, const glm::vec3 &wi) const
    {
        glm::vec3 wh = wi + wo;
        if (wh.x == 0 && wh.y == 0 && wh.z == 0)
            return glm::vec3(0.);
        wh = glm::normalize(wh);
        float cosThetaD = glm::dot(wi, wh);

        return R * SchlickWeight(cosThetaD);
    }

    class DisneyClearcoat : public BxDF
    {
    public:
        DisneyClearcoat(float weight, float gloss)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
              weight(weight),
              gloss(gloss) {}
        glm::vec3 f(const glm::vec3 &wo, const glm::vec3 &wi) const;
        glm::vec3 Sample_f(const glm::vec3 &wo, glm::vec3 *wi, const glm::vec2 &u,
                           float *pdf, BxDFType *sampledType) const;
        float Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const;

    private:
        float weight, gloss;
    };

    inline float GTR1(float cosTheta, float alpha)
    {
        float alpha2 = alpha * alpha;
        return (alpha2 - 1) /
               (glm::pi<float>() * std::log(alpha2) * (1 + (alpha2 - 1) * cosTheta * cosTheta));
    }

    // Smith masking/shadowing term.
    inline float smithG_GGX(float cosTheta, float alpha)
    {
        float alpha2 = alpha * alpha;
        float cosTheta2 = cosTheta * cosTheta;
        return 1 / (cosTheta + sqrt(alpha2 + cosTheta2 - alpha2 * cosTheta2));
    }

    glm::vec3 DisneyClearcoat::f(const glm::vec3 &wo, const glm::vec3 &wi) const
    {
        glm::vec3 wh = wi + wo;
        if (wh.x == 0 && wh.y == 0 && wh.z == 0)
            return glm::vec3(0.);
        wh = glm::normalize(wh);

        float Dr = GTR1(AbsCosTheta(wh), gloss);
        float Fr = FrSchlick(.04, glm::dot(wo, wh));
        float Gr =
            smithG_GGX(AbsCosTheta(wo), .25f) * smithG_GGX(AbsCosTheta(wi), .25);

        return glm::vec3(weight) * Gr * Fr * Dr / 4.0f;
    }

    glm::vec3 DisneyClearcoat::Sample_f(const glm::vec3 &wo, glm::vec3 *wi,
                                        const glm::vec2 &u, float *pdf,
                                        BxDFType *sampledType) const
    {
        if (wo.z == 0)
            return glm::vec3(0.f);

        float alpha2 = gloss * gloss;
        float cosTheta = std::sqrt(
            std::max(float(0), (1 - std::pow(alpha2, 1 - u[0])) / (1 - alpha2)));
        float sinTheta = std::sqrt(std::max((float)0, 1 - cosTheta * cosTheta));
        float phi = 2 * glm::pi<float>() * u[1];
        glm::vec3 wh = SphericalDirection(sinTheta, cosTheta, phi);
        if (!SameHemisphere(wo, wh))
            wh = -wh;

        *wi = Reflect(wo, wh);
        if (!SameHemisphere(wo, *wi))
            return glm::vec3(0.f);

        *pdf = Pdf(wo, *wi);
        return f(wo, *wi);
    }

    float DisneyClearcoat::Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const
    {
        if (!SameHemisphere(wo, wi))
            return 0;

        glm::vec3 wh = wi + wo;
        if (wh.x == 0 && wh.y == 0 && wh.z == 0)
            return 0;
        wh = glm::normalize(wh);
        float Dr = GTR1(AbsCosTheta(wh), gloss);
        return Dr * AbsCosTheta(wh) / (4 * glm::dot(wo, wh));
    }

    class DisneyFresnel : public Fresnel
    {
    public:
        DisneyFresnel(const glm::vec3 &R0, float metallic, float eta)
            : R0(R0), metallic(metallic), eta(eta) {}
        glm::vec3 Evaluate(float cosI) const
        {
            return glm::mix(glm::vec3(metallic), glm::vec3(FrDielectric(cosI, 1, eta)),
                            FrSchlick(R0, cosI));
        }

    private:
        const glm::vec3 R0;
        const float metallic, eta;
    };

    class DisneyMicrofacetDistribution : public BeckmannDistribution
    {
    public:
        DisneyMicrofacetDistribution(float alphax, float alphay)
            : BeckmannDistribution(alphax, alphay) {}

        const float G(const glm::vec3 &wo, const glm::vec3 &wi) const
        {
            // Disney uses the separable masking-shadowing model.
            return G1(wo) * G1(wi);
        }
    };

    const std::unique_ptr<BSDF> PrincipledBSDF::GetBSDF(const IntersectInfo &intersect_info) const
    {
        std::unique_ptr<BSDF> bsdf = std::make_unique<BSDF>(intersect_info);

        glm::vec3 c = _albedo->Sample(intersect_info._uv);
        float metallicWeight = metallic->Sample(intersect_info._uv);
        float e = eta->Sample(intersect_info._uv);
        float strans = specTrans->Sample(intersect_info._uv);
        float diffuseWeight = (1.0f - metallicWeight) * (1.0f - strans);
        float dt = diffTrans->Sample(intersect_info._uv) / 2.0f;
        float rough = roughness->Sample(intersect_info._uv);
        float lum = c.y;
        glm::vec3 Ctint = lum > 0.0f ? (c / lum) : glm::vec3(1.0f);
        float sheenWeight = sheen->Sample(intersect_info._uv);
        glm::vec3 Csheen;
        if (sheenWeight > 0.0f)
        {
            float stint = sheenTint->Sample(intersect_info._uv);
            Csheen = glm::mix(glm::vec3(stint), glm::vec3(1.0f), Ctint);
        }

        if (diffuseWeight > 0.0f)
        {
            if (thin)
            {
                float flat = flatness->Sample(intersect_info._uv);
                bsdf->Add(std::make_shared<DisneyDiffuse>(diffuseWeight * (1.0f - flat) * (1.0f - dt) * c));
                bsdf->Add(std::make_shared<DisneyFakeSS>(diffuseWeight * flat * (1.0f - dt) * c, rough));
            }
            else
            {
                bsdf->Add(std::make_shared<DisneyDiffuse>(diffuseWeight * c));
            }

            bsdf->Add(std::make_shared<DisneyRetro>(diffuseWeight * c, rough));

            if (sheenWeight > 0.0f)
                bsdf->Add(std::make_shared<DisneySheen>(diffuseWeight * sheenWeight * Csheen));
        }

        float aspect = std::sqrt(1 - anisotropic->Sample(intersect_info._uv) * .9);
        float ax = std::max(float(.001), sqr(rough) / aspect);
        float ay = std::max(float(.001), sqr(rough) * aspect);
        std::shared_ptr<MicrofacetDistribution> distrib =
            std::make_shared<DisneyMicrofacetDistribution>(ax, ay);

        float specTint = specularTint->Sample(intersect_info._uv);
        glm::vec3 Cspec0 =
            glm::mix(glm::vec3(metallicWeight),
                     SchlickR0FromEta(e) * glm::mix(glm::vec3(specTint), glm::vec3(1.f), Ctint), c);
        std::shared_ptr<Fresnel> fresnel =
            std::make_shared<DisneyFresnel>(Cspec0, metallicWeight, e);
        bsdf->Add(
            std::make_shared<MicrofacetReflection>(glm::vec3(1.), distrib, fresnel));

        float cc = clearcoat->Sample(intersect_info._uv);
        if (cc > 0)
        {
            bsdf->Add(std::make_shared<DisneyClearcoat>(
                cc, glm::mix(clearcoatGloss->Sample(intersect_info._uv), .1f, .001f)));
        }

        if (strans > 0)
        {
            glm::vec3 T = strans * glm::sqrt(c);
            if (thin)
            {
                float rscaled = (0.65f * e - 0.35f) * rough;
                float ax = std::max(float(.001), sqr(rscaled) / aspect);
                float ay = std::max(float(.001), sqr(rscaled) * aspect);
                std::shared_ptr<MicrofacetDistribution> scaledDistrib =
                    std::make_shared<BeckmannDistribution>(ax, ay);
                bsdf->Add(std::make_shared<MicrofacetTransmission>(
                    T, scaledDistrib, 1.f, e));
            }
            else
                bsdf->Add(std::make_shared<MicrofacetTransmission>(
                    T, distrib, 1.f, e));
        }
        if (thin)
        {
            bsdf->Add(std::make_shared<LambertianTransmission>(dt * c));
        }
        return bsdf;
    }
}