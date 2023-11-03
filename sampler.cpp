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

const float rendertoy::PowerHeuristic(int nf, float f_pdf, int ng, float g_pdf)
{
    float f = static_cast<float>(nf) * f_pdf;
    float g = static_cast<float>(ng) * g_pdf;
    return (f * f) / (f * f + g * g);
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
        bins[un.index].alias = ov.index;

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
    int offset = std::min<int>(u * bins.size(), bins.size() - 1);
    float up = std::min<float>(u * bins.size(), ONE_MINUS_EPSILON);

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
