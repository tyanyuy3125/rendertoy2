#include "material.h"
#include "integrater.h"

#include <glm/gtc/constants.hpp>

const glm::vec3 rendertoy::DiffuseBSDF::Eval(const IntersectInfo &intersect_info) const
{
    return glm::vec3(1.0f / glm::pi<float>());
}

const glm::vec3 rendertoy::DiffuseBSDF::Sample(const IntersectInfo &intersect_info) const
{
    return UniformSampleHemisphere();
}
