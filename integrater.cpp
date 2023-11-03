#include "integrater.h"

#include <cmath>
#include <stdexcept>
#include <algorithm>

const glm::vec3 rendertoy::UniformSampleHemisphere()
{
    float u0 = glm::linearRand(0.0f, 1.0f);
    float u1 = glm::linearRand(0.0f, 1.0f);

    float z = u0;
    float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    float phi = 2 * glm::pi<float>() * u1;
    return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

const float rendertoy::PowerHeuristic(int nf, float f_pdf, int ng, float g_pdf)
{
    float f = static_cast<float>(nf) * f_pdf;
    float g = static_cast<float>(ng) * g_pdf;
    return (f * f) / (f * f + g * g);
}
