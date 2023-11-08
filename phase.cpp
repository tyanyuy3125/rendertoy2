#include "phase.h"

#include "sampler.h"

const glm::vec3 rendertoy::IsotropicPhaseFunction::Sample_p(const glm::vec3 &wo, float *p)
{
    *p = 0.25f * glm::one_over_pi<float>();
    return UniformSampleSphere();
}