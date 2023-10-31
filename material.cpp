#include "material.h"
#include "integrater.h"

#include <glm/gtc/constants.hpp>

const glm::vec3 rendertoy::DiffuseBSDF::EvalEmissive(const IntersectInfo &intersect_info) const
{
    return glm::vec3(0.0f);
}

const glm::vec3 rendertoy::DiffuseBSDF::Eval(const IntersectInfo &intersect_info) const
{
    return glm::vec3(_albedo->Sample(intersect_info._uv)) * glm::vec3(1.0f / glm::pi<float>());
}

const glm::vec3 rendertoy::DiffuseBSDF::Sample(const IntersectInfo &intersect_info) const
{
    return UniformSampleHemisphere();
}

const glm::vec3 rendertoy::Emissive::EvalEmissive(const IntersectInfo &intersect_info) const
{
    return glm::vec3(_albedo->Sample(intersect_info._uv)) * _strength->Sample(intersect_info._uv);
}

const glm::vec3 rendertoy::Emissive::Eval(const IntersectInfo &intersect_info) const
{
    return glm::vec3(0.0f);
}

const glm::vec3 rendertoy::Emissive::Sample(const IntersectInfo &intersect_info) const
{
    return UniformSampleHemisphere();
}
