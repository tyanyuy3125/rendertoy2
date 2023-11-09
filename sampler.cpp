#include "sampler.h"

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>

const glm::vec3 rendertoy::UniformSampleHemisphere()
{
    float u0 = glm::linearRand(0.0f, 1.0f);
    float u1 = glm::linearRand(0.0f, 1.0f);

    float z = u0;
    float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    float phi = 2.0f * glm::pi<float>() * u1;
    return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

const float rendertoy::UniformSampleHemispherePdf()
{
    return 1.0f / glm::two_pi<float>();
}

const glm::vec2 rendertoy::ConcentricSampleDisk()
{
    glm::vec2 u{glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f)};
    // Map uniform random numbers to $[-1,1]^2$
    glm::vec2 uOffset = 2.f * u - glm::vec2(1.0f);

    // Handle degeneracy at the origin
    if (uOffset.x == 0.0f && uOffset.y == 0.0f)
        return glm::vec2(0.0f);

    // Apply concentric mapping to point
    float theta, r;
    if (std::abs(uOffset.x) > std::abs(uOffset.y))
    {
        r = uOffset.x;
        theta = glm::quarter_pi<float>() * (uOffset.y / uOffset.x);
    }
    else
    {
        r = uOffset.y;
        theta = glm::half_pi<float>() - glm::quarter_pi<float>() * (uOffset.x / uOffset.y);
    }
    return r * glm::vec2(std::cos(theta), std::sin(theta));
}

const glm::vec3 rendertoy::CosineSampleHemisphere()
{
    glm::vec2 d = ConcentricSampleDisk();
    float z = std::sqrt(std::max(0.0f, 1.0f - d.x * d.x - d.y * d.y));
    return glm::vec3(d.x, d.y, z);
}

const float rendertoy::CosineSampleHemispherePdf(float cosTheta) { return cosTheta * glm::one_over_pi<float>(); }

const float rendertoy::PowerHeuristic(int nf, float f_pdf, int ng, float g_pdf)
{
    float f = static_cast<float>(nf) * f_pdf;
    float g = static_cast<float>(ng) * g_pdf;
    return (f * f) / (f * f + g * g);
}

const glm::vec3 rendertoy::UniformSampleSphere()
{
    float z = 1 - 2 * glm::linearRand(0.0f, 1.0f);
    float r = std::sqrt(1 - z * z);
    assert(1 - z * z < 0);
    float phi = 2 * glm::pi<float>() * glm::linearRand(0.0f, 1.0f);
    return {r * std::cos(phi), r * std::sin(phi), z};
}

const float rendertoy::SampleExponential(const float u, const float a)
{
    return -std::log(1 - u) / a;
}

const int rendertoy::SampleDiscrete(std::span<const float> weights)
{
    std::vector<float> cdf(weights.size());
    std::partial_sum(weights.begin(), weights.end(), cdf.begin());
    float random_value = glm::linearRand<float>(0.0f, cdf.back());
    auto it = std::lower_bound(cdf.begin(), cdf.end(), random_value);
    return std::distance(cdf.begin(), it);
}

rendertoy::AliasTable::AliasTable(std::span<float> weights)
    : bins(weights.size())
{
    float sum = std::accumulate(weights.begin(), weights.end(), 0.f);
    for (size_t i = 0; i < weights.size(); ++i)
    {
        bins[i].p = weights[i] / sum;
    }

    struct Outcome
    {
        float pHat;
        size_t index;
    };
    std::vector<Outcome> under, over;
    for (size_t i = 0; i < bins.size(); ++i)
    {
        float pHat = bins[i].p * bins.size();
        if (pHat < 1)
        {
            under.push_back(Outcome{pHat, i});
        }
        else
        {
            over.push_back(Outcome{pHat, i});
        }
    }

    while (!under.empty() && !over.empty())
    {
        Outcome un = under.back(), ov = over.back();
        under.pop_back();
        over.pop_back();

        bins[un.index].q = un.pHat;
        bins[un.index].alias = static_cast<int>(ov.index);

        float pExcess = un.pHat + ov.pHat - 1.0f;
        if (pExcess < 1.0f)
        {
            under.push_back(Outcome{pExcess, ov.index});
        }
        else
        {
            over.push_back(Outcome{pExcess, ov.index});
        }
    }

    while (!under.empty())
    {
        Outcome un = under.back();
        under.pop_back();

        bins[un.index].q = 1;
        bins[un.index].alias = -1;
    }

    while (!over.empty())
    {
        Outcome ov = over.back();
        over.pop_back();

        bins[ov.index].q = 1;
        bins[ov.index].alias = -1; // 这种实现要求在采样时引入 ONE_MINUS_EPSILON
    }
}

int rendertoy::AliasTable::Sample(const float u, float *pmf, float *u_remapped) const
{
    int offset = std::min<int>(static_cast<int>(u * bins.size()), static_cast<int>(bins.size()) - 1);
    float up = std::min<float>(u * bins.size() - offset, ONE_MINUS_EPSILON);

    if (up < bins[offset].q)
    {
        if (pmf)
            *pmf = bins[offset].p;
        if (u_remapped)
            *u_remapped = std::min<float>(up / bins[offset].q, ONE_MINUS_EPSILON);
        return offset;
    }
    else
    {
        int alias = bins[offset].alias;
        if (pmf)
            *pmf = bins[alias].p;
        if (u_remapped)
            *u_remapped = std::min<float>((up - bins[offset].q) / (1 - bins[offset].q),
                                          ONE_MINUS_EPSILON);
        return alias;
    }
}

rendertoy::Distribution2D::Distribution2D(const float *func, int nu, int nv) {
    pConditionalV.reserve(nv);
    for (int v = 0; v < nv; ++v) {
        pConditionalV.emplace_back(new Distribution1D(&func[v * nu], nu));
    }
    std::vector<float> marginalFunc;
    marginalFunc.reserve(nv);
    for (int v = 0; v < nv; ++v)
        marginalFunc.push_back(pConditionalV[v]->funcInt);
    pMarginal.reset(new Distribution1D(&marginalFunc[0], nv));
}