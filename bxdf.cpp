#include "bxdf.h"
#include "sampler.h"
#include "intersectinfo.h"
#include "fresnel.h"
#include "microfacet.h"

glm::vec3 rendertoy::BxDF::Sample_f(const glm::vec3 &wo, glm::vec3 *wi, float *pdf, BxDFType *sampled_type) const
{
    // Cosine-sample the hemisphere, flipping the direction if necessary
    *wi = CosineSampleHemisphere();
    if (wo.z < 0)
        wi->z *= -1;
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

float rendertoy::BxDF::Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * glm::one_over_pi<float>() : 0.0f;
}

glm::vec3 rendertoy::LambertianReflection::f(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return R * glm::one_over_pi<float>();
}

glm::vec3 rendertoy::OrenNayar::f(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    float sinThetaI = SinTheta(wi);
    float sinThetaO = SinTheta(wo);
    float maxCos = 0.0f;
    if (sinThetaI > 1e-4f && sinThetaO > 1e-4f)
    {
        float sinPhiI = SinPhi(wi), cosPhiI = CosPhi(wi);
        float sinPhiO = SinPhi(wo), cosPhiO = CosPhi(wo);
        float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
        maxCos = std::max(0.0f, dCos);
    }
    float sinAlpha, tanBeta;
    if (AbsCosTheta(wi) > AbsCosTheta(wo))
    {
        sinAlpha = sinThetaO;
        tanBeta = sinThetaI / AbsCosTheta(wi);
    }
    else
    {
        sinAlpha = sinThetaI;
        tanBeta = sinThetaO / AbsCosTheta(wo);
    }
    return R * glm::vec3(glm::one_over_pi<float>() * (A + B * maxCos * sinAlpha * tanBeta));
}

rendertoy::BSDF::BSDF(const IntersectInfo &intersect_info, float eta)
    : _ltw(intersect_info.GenerateSurfaceCoordinates()), _eta(eta)
{
}

int rendertoy::BSDF::NumComponents(BxDFType flags) const
{
    int num = 0;
    for (int i = 0; i < nBxDFs; ++i)
        if (bxdfs[i]->MatchesFlags(flags))
            ++num;
    return num;
}

const glm::vec3 rendertoy::BSDF::f(const glm::vec3 &wo_w, const glm::vec3 &wi_w, BxDFType flags) const
{
    const glm::vec3 wo = WorldToLocal(wo_w);
    const glm::vec3 wi = WorldToLocal(wi_w);
    if (wo.z == 0.0f)
    {
        return glm::vec3(0.0f);
    }
    bool reflect = wo.z * wi.z > 0;
    glm::vec3 ret(0.0f);
    for (int i = 0; i < nBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(flags) &&
            ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
             (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
        {
            ret += bxdfs[i]->f(wo, wi);
        }
    }
    return ret;
}

const glm::vec3 rendertoy::BSDF::Sample_f(const glm::vec3 &wo_w, glm::vec3 *wi_w, float *pdf, BxDFType type, BxDFType *sampled_type) const
{
    glm::vec2 u{glm::linearRand<float>(0.0f, 1.0f), glm::linearRand<float>(0.0f, 1.0f)};
    int matchingComps = NumComponents(type);
    if (matchingComps == 0)
    {
        *pdf = 0;
        if (sampled_type)
            *sampled_type = BxDFType(0);
        return glm::vec3(0.0f);
    }
    int comp =
        std::min(static_cast<int>(std::floor(u[0] * matchingComps)), matchingComps - 1);

    // Get _BxDF_ pointer for chosen component
    std::shared_ptr<BxDF> bxdf = nullptr;
    int count = comp;
    for (int i = 0; i < nBxDFs; ++i)
        if (bxdfs[i]->MatchesFlags(type) && count-- == 0)
        {
            bxdf = bxdfs[i];
            break;
        }

    // Remap _BxDF_ sample _u_ to $[0,1)^2$
    // glm::vec2 uRemapped(std::min(u[0] * matchingComps - comp, ONE_MINUS_EPSILON),
    //                   u[1]);

    // Sample chosen _BxDF_
    glm::vec3 wi, wo = WorldToLocal(wo_w);
    if (wo.z == 0)
        return glm::vec3(0.0f);
    *pdf = 0.0f;
    if (sampled_type)
        *sampled_type = bxdf->type;
    glm::vec3 f = bxdf->Sample_f(wo, &wi, pdf, sampled_type);
    if (*pdf == 0)
    {
        if (sampled_type)
            *sampled_type = BxDFType(0);
        return glm::vec3(0.0f);
    }
    *wi_w = LocalToWorld(wi);

    // Compute overall PDF with all matching _BxDF_s
    if (!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
        for (int i = 0; i < nBxDFs; ++i)
            if (bxdfs[i] != bxdf && bxdfs[i]->MatchesFlags(type))
                *pdf += bxdfs[i]->Pdf(wo, wi);
    if (matchingComps > 1)
        *pdf /= matchingComps;

    // Compute value of BSDF for sampled direction
    if (!(bxdf->type & BSDF_SPECULAR))
    {
        bool reflect = wi.z * wo.z > 0;
        f = glm::vec3(0.0f);
        for (int i = 0; i < nBxDFs; ++i)
            if (bxdfs[i]->MatchesFlags(type) &&
                ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
                 (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
                f += bxdfs[i]->f(wo, wi);
    }
    return f;
}

const float rendertoy::BSDF::Pdf(const glm::vec3 &wo_w, const glm::vec3 &wi_w, BxDFType flags)
{
    if (nBxDFs == 0.f)
        return 0.f;
    const glm::vec3 wo = WorldToLocal(wo_w), wi = WorldToLocal(wi_w);
    if (wo.z == 0)
        return 0.;
    float pdf = 0.f;
    int matchingComps = 0;
    for (int i = 0; i < nBxDFs; ++i)
        if (bxdfs[i]->MatchesFlags(flags))
        {
            ++matchingComps;
            pdf += bxdfs[i]->Pdf(wo, wi);
        }
    float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
    return v;
}

glm::vec3 rendertoy::SpecularReflection::Sample_f(const glm::vec3 &wo, glm::vec3 *wi, float *pdf, BxDFType *sampledType) const
{
    *wi = glm::vec3(-wo.x, -wo.y, wo.z);
    *pdf = 1;
    return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
}

glm::vec3 rendertoy::MicrofacetReflection::f(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
    glm::vec3 wh = wi + wo;
    // Handle degenerate cases for microfacet reflection
    if (cosThetaI == 0 || cosThetaO == 0)
        return glm::vec3(0.);
    if (wh.x == 0 && wh.y == 0 && wh.z == 0)
        return glm::vec3(0.);
    wh = glm::normalize(wh);
    // For the Fresnel call, make sure that wh is in the same hemisphere
    // as the surface normal, so that TIR is handled correctly.
    glm::vec3 F = fresnel->Evaluate(glm::dot(wi, Faceforward(wh, glm::vec3(0.0f, 0.0f, 1.0f))));
    return R * distribution->D(wh) * distribution->G(wo, wi) * F /
           (4.0f * cosThetaI * cosThetaO);
}

glm::vec3 rendertoy::MicrofacetReflection::Sample_f(const glm::vec3 &wo, glm::vec3 *wi, const glm::vec2 &u, float *pdf, BxDFType *sampledType) const
{
    // Sample microfacet orientation $\wh$ and reflected direction $\wi$
    if (wo.z == 0)
        return glm::vec3(0.0f);
    glm::vec3 wh = distribution->Sample_wh(wo, u);
    if (glm::dot(wo, wh) < 0)
        return glm::vec3(0.0f); // Should be rare
    *wi = Reflect(wo, wh);
    if (!SameHemisphere(wo, *wi))
        return glm::vec3(0.f);

    // Compute PDF of _wi_ for microfacet reflection
    *pdf = distribution->Pdf(wo, wh) / (4 * glm::dot(wo, wh));
    return f(wo, *wi);
}

float rendertoy::MicrofacetReflection::Pdf(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    if (!SameHemisphere(wo, wi))
        return 0;
    glm::vec3 wh = glm::normalize(wo + wi);
    return distribution->Pdf(wo, wh) / (4 * glm::dot(wo, wh));
}
