#include "medium.h"
#include "intersectinfo.h"
#include "logger.h"

const glm::vec3 rendertoy::HomogeneousMedium::Tr(const float t) const
{
    return glm::exp(-(_sigma_a + _sigma_s) * t);
}

const glm::vec3 rendertoy::HomogeneousMedium::Sample(const glm::vec3 &o, const glm::vec3 &d, const float tmax, VolumeInteraction &v_i) const
{
    v_i._valid = false;
    glm::vec3 sigma_t = _sigma_a + _sigma_s;
    float dist = -std::log(1.0f - glm::linearRand(0.0f, 1.0f)) / sigma_t[0];
    float t = std::min(dist, tmax);
    bool sampledMedium = t < tmax;
    if (sampledMedium)
    {
        v_i._coord = o + t * d;
        v_i._wo = -d;
        v_i._phase_func = _phase_func;
        v_i._valid = true;
    }

    glm::vec3 Tr = glm::exp(-sigma_t * t);

    glm::vec3 density = sampledMedium ? (sigma_t * Tr) : Tr;
    float pdf = glm::compAdd(density);
    pdf /= 3.0f;
    if (pdf == 0.0f)
    {
        pdf = 1.0f;
    }
    return sampledMedium ? (Tr * _sigma_s / pdf) : (Tr / pdf);
}
