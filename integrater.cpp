#include "integrater.h"

#include <glm/gtc/random.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <stdexcept>

const glm::vec3 rendertoy::UniformSampleHemisphere()
{
    float u1 = glm::linearRand(0.0f, 1.0f);
    float u2 = glm::linearRand(0.0f, 1.0f);

    float phi = 2.0f * glm::pi<float>() * u1;
    float cosTheta = u2;
    float sinTheta = std::sqrt(1.0f - u2 * u2);

    float x = sinTheta * std::cos(phi);
    float y = cosTheta;
    float z = sinTheta * std::sin(phi);

    return glm::vec3(x, y, z);
}

const float rendertoy::PowerHeuristic(int nf, float f_pdf, int ng, float g_pdf)
{
    float f = static_cast<float>(nf) * f_pdf;
    float g = static_cast<float>(ng) * g_pdf;
    return (f * f) / (f * f + g * g);
}
