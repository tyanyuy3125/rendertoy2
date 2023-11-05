#include "material.h"
#include "sampler.h"
#include "intersectinfo.h"
#include "texture.h"
#include "fresnel.h"

#include <glm/gtc/constants.hpp>

const glm::vec3 rendertoy::DiffuseBSDF::EvalLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv, float &pdf) const
{
    pdf = 1.0f / (2.0f * glm::pi<float>());
    if (_use_lambertian)
        return glm::vec3(_albedo->Sample(uv)) * glm::vec3(1.0f / glm::pi<float>());
    else
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
        return glm::vec3(_albedo->Sample(uv)) * glm::vec3(glm::one_over_pi<float>() * (A + B * maxCos * sinAlpha * tanBeta));
    }
}

const glm::vec3 rendertoy::DiffuseBSDF::SampleLocal(const glm::vec3 &wo, const glm::vec2 &uv, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const
{
    flags = Diffuse;
    const glm::vec3 wi = UniformSampleHemisphere();
    bsdf = EvalLocal(wo, wi, uv, pdf);
    return wi;
}

const glm::vec3 rendertoy::Emissive::EvalLocal(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec2 &uv, float &pdf) const
{
    pdf = 1.0f / (2.0f * glm::pi<float>());
    return glm::vec3(_albedo->Sample(uv)) * glm::vec3(1.0f / glm::pi<float>());
}

const glm::vec3 rendertoy::Emissive::SampleLocal(const glm::vec3 &wo, const glm::vec2 &uv, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const
{
    flags = Diffuse;
    const glm::vec3 wi = UniformSampleHemisphere();
    bsdf = EvalLocal(wo, wi, uv, pdf);
    return wi;
}

const glm::vec3 rendertoy::Emissive::EvalEmissive(const glm::vec2 &uv) const
{
    return glm::vec3(_albedo->Sample(uv)) * _strength->Sample(uv);
}

const glm::vec3 rendertoy::IMaterial::Eval(const IntersectInfo &intersect_info, const glm::vec3 &wi, float &pdf) const
{
    const glm::mat3 wtl = glm::transpose(intersect_info.GenerateSurfaceCoordinates());
    const glm::vec3 wo_local = wtl * intersect_info._wo;
    const glm::vec3 wi_local = wtl * wi;
    return EvalLocal(wo_local, wi_local, intersect_info._uv, pdf);
}

const glm::vec3 rendertoy::IMaterial::Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const
{
    const glm::mat3 wtl = glm::transpose(intersect_info.GenerateSurfaceCoordinates());
    const glm::vec3 wo_local = wtl * intersect_info._wo;
    const glm::vec3 wi_local = SampleLocal(wo_local, intersect_info._uv, pdf, bsdf, flags);
    return glm::transpose(wtl) * wi_local;
}

const glm::vec3 rendertoy::SpecularBSDF::EvalLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv, float &pdf) const
{
    return glm::vec3(0.0f);
}

const glm::vec3 rendertoy::SpecularBSDF::SampleLocal(const glm::vec3 &wo, const glm::vec2 &uv, float &pdf, glm::vec3 &bsdf, BxDFFlags &flags) const
{
    flags = Specular;
    pdf = 1.0f;
    const glm::vec3 wi = glm::vec3(-wo.x, -wo.y, wo.z);
    bsdf = glm::vec3(_albedo->Sample(uv)) * _fresnel->Evaluate(CosTheta(wi)) / AbsCosTheta(wi);
    return wi;
}

const float rendertoy::SpecularBSDF::PdfLocal(const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec2 &uv) const
{
    return 0.0f;
}

rendertoy::SpecularBSDF::SpecularBSDF(const std::shared_ptr<ISamplableColor> &albedo)
: IMaterial(albedo), _fresnel(std::make_shared<FresnelNoOp>())
{
}
