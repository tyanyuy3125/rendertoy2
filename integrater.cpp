#include "integrater.h"

#include <glm/gtc/random.hpp>
#include <glm/gtc/constants.hpp>

const glm::vec3 rendertoy::UniformSampleHemisphere()
{
    float u1 = glm::linearRand(0.0f, 1.0f);
    float u2 = glm::linearRand(0.0f, 1.0f);

    float phi = 2.0f * glm::pi<float>() * u1;
    float cosTheta = u2;
    float sinTheta = std::sqrt(1.0f - u2 * u2);

    float x = sinTheta * std::cos(phi);
    float y = sinTheta * std::sin(phi);
    float z = cosTheta;

    return glm::vec3(x, y, z);
}