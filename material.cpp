#include "material.h"
#include "sampler.h"
#include "intersectinfo.h"
#include "texture.h"

#include <glm/gtc/constants.hpp>

const glm::vec3 rendertoy::DiffuseBSDF::EvalLocal(const glm::vec3 &wi, const glm::vec3 &wo, float &pdf) const
{
    pdf = 1.0f / (2.0f * glm::pi<float>());
    if (_use_lambertian)
        return glm::vec3(1.0f / glm::pi<float>());
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
        return glm::vec3(glm::one_over_pi<float>() * (A + B * maxCos * sinAlpha * tanBeta));
    }
}

const glm::vec3 rendertoy::DiffuseBSDF::EvalEmissive(const glm::vec2 &uv) const
{
    return glm::vec3(0.0f);
}

// const glm::vec3 rendertoy::DiffuseBSDF::Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const
// {
//     pdf = 1.0f / (2.0f * glm::pi<float>());
//     return glm::vec3(_albedo->Sample(intersect_info._uv)) * glm::vec3(1.0f / glm::pi<float>());
// }

const glm::vec3 rendertoy::DiffuseBSDF::Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf) const
{
    // In world space.
    glm::vec3 ret = intersect_info.GenerateSurfaceCoordinates() * UniformSampleHemisphere();
    bsdf = Eval(intersect_info, ret, pdf);
    return ret;
}

const glm::vec3 rendertoy::Emissive::EvalLocal(const glm::vec3 &wi, const glm::vec3 &wo, float &pdf) const
{
    pdf = 1.0f / (2.0f * glm::pi<float>());
    return glm::vec3(1.0f / glm::pi<float>());
}

const glm::vec3 rendertoy::Emissive::EvalEmissive(const glm::vec2 &uv) const
{
    return glm::vec3(_albedo->Sample(uv)) * _strength->Sample(uv);
}

// const glm::vec3 rendertoy::Emissive::Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const
// {
//     pdf = 1.0f / (2.0f * glm::pi<float>());
//     return glm::vec3(_albedo->Sample(intersect_info._uv)) * glm::vec3(1.0f / glm::pi<float>());
// }

const glm::vec3 rendertoy::Emissive::Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf) const
{
    glm::vec3 ret = intersect_info.GenerateSurfaceCoordinates() * UniformSampleHemisphere();
    bsdf = Eval(intersect_info, ret, pdf);
    return ret;
}

const glm::vec3 rendertoy::IMaterial::Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const
{
    const glm::mat3 wtl = glm::transpose(intersect_info.GenerateSurfaceCoordinates());
    const glm::vec3 in_local = wtl * intersect_info._in;
    const glm::vec3 out_local = wtl * out;
    return glm::vec3(_albedo->Sample(intersect_info._uv)) * EvalLocal(in_local, out_local, pdf);
}
