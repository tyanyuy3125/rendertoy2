#include "material.h"
#include "sampler.h"
#include "intersectinfo.h"
#include "texture.h"
#include "fresnel.h"
#include "bxdf.h"
#include "microfacet.h"

#include <glm/gtc/constants.hpp>

const glm::vec3 rendertoy::Emissive::EvalEmissive(const glm::vec2 &uv) const
{
    return glm::vec3(_albedo->Sample(uv)) * _strength->Sample(uv);
}

const std::unique_ptr<rendertoy::BSDF> rendertoy::Emissive::GetBSDF(const IntersectInfo &intersect_info) const
{
    const glm::vec3 albedo = _albedo->Sample(intersect_info._uv);
    std::unique_ptr<BSDF> bsdf = std::make_unique<BSDF>(intersect_info);
    bsdf->Add(std::make_shared<LambertianReflection>(albedo));
    return bsdf;
}

const std::unique_ptr<rendertoy::BSDF> rendertoy::DiffuseBSDF::GetBSDF(const IntersectInfo &intersect_info) const
{
    const float sigma = _sigma->Sample(intersect_info._uv);
    const glm::vec3 albedo = _albedo->Sample(intersect_info._uv);
    std::unique_ptr<BSDF> bsdf = std::make_unique<BSDF>(intersect_info);
    if (sigma == 0.0f)
    {
        bsdf->Add(std::make_shared<LambertianReflection>(albedo));
    }
    else
    {
        bsdf->Add(std::make_shared<OrenNayar>(albedo, sigma));
    }
    return bsdf;
}

const std::unique_ptr<rendertoy::BSDF> rendertoy::SpecularBSDF::GetBSDF(const IntersectInfo &intersect_info) const
{
    const glm::vec3 albedo = _albedo->Sample(intersect_info._uv);
    std::unique_ptr<BSDF> bsdf = std::make_unique<BSDF>(intersect_info);
    bsdf->Add(std::make_shared<SpecularReflection>(albedo, std::make_shared<FresnelNoOp>()));
    return bsdf;
}

const std::unique_ptr<rendertoy::BSDF> rendertoy::MetalBSDF::GetBSDF(const IntersectInfo &intersect_info) const
{
    const glm::vec3 albedo = _albedo->Sample(intersect_info._uv);
    const glm::vec3 eta = _eta->Sample(intersect_info._uv);
    const glm::vec3 k = _k->Sample(intersect_info._uv);
    float u_roughness = _u_roughness->Sample(intersect_info._uv);
    float v_roughness = _v_roughness->Sample(intersect_info._uv);
    if (_remap_roughness)
    {
        u_roughness = BeckmannDistribution::RoughnessToAlpha(u_roughness);
        v_roughness = BeckmannDistribution::RoughnessToAlpha(v_roughness);
    }
    std::unique_ptr<BSDF> bsdf = std::make_unique<BSDF>(intersect_info);
    bsdf->Add(std::make_shared<MicrofacetReflection>(albedo, std::make_shared<BeckmannDistribution>(u_roughness, v_roughness), std::make_shared<FresnelConductor>(glm::vec3(1.0f), eta, k)));
    return bsdf;
}

const std::unique_ptr<rendertoy::BSDF> rendertoy::RefractionBSDF::GetBSDF(const IntersectInfo &intersect_info) const
{
    const float eta = _eta->Sample(intersect_info._uv);
    std::unique_ptr<BSDF> bsdf = std::make_unique<BSDF>(intersect_info, eta);
    const glm::vec3 albedo = _albedo->Sample(intersect_info._uv);
    const glm::vec3 transmissive = _transmissive->Sample(intersect_info._uv);
    float u_roughness, v_roughness;
    if (!_u_roughness)
    {
        u_roughness = 0.0f;
    }
    else
    {
        u_roughness = _u_roughness->Sample(intersect_info._uv);
    }
    if (!_v_roughness)
    {
        v_roughness = 0.0f;
    }
    else
    {
        v_roughness = _v_roughness->Sample(intersect_info._uv);
    }

    if (u_roughness == 0.0f && v_roughness == 0.0f)
    {
        // bsdf->Add(std::make_shared<SpecularTransmission>(transmissive, 1.0f, eta));
        bsdf->Add(std::make_shared<FresnelSpecular>(albedo, transmissive, 1.0f, eta));
    }
    else
    {
        if (_remap_roughness)
        {
            u_roughness = BeckmannDistribution::RoughnessToAlpha(u_roughness);
            v_roughness = BeckmannDistribution::RoughnessToAlpha(v_roughness);
        }
        auto distrib = std::make_shared<BeckmannDistribution>(u_roughness, v_roughness);
        if (albedo != glm::vec3(0.0f))
        {
            std::shared_ptr<Fresnel> fresnel = std::make_shared<FresnelDielectric>(1.f, eta);
            bsdf->Add(std::make_shared<MicrofacetReflection>(albedo, distrib, fresnel));
        }
        if (transmissive != glm::vec3(0.0f))
        {
            bsdf->Add(std::make_shared<MicrofacetTransmission>(transmissive, distrib, 1.f, eta));
        }
    }
    return bsdf;
}
