#pragma once

#include <vector>
#include <span>

#include "rendertoy_internal.h"

namespace rendertoy
{
    const glm::vec3 UniformSampleHemisphere();

    const float PowerHeuristic(int nf, float f_pdf, int ng, float g_pdf);

    class AliasTable
    {
    public:
        AliasTable(std::span<const float> weights);

        int Sample(const float u, float *pmf = nullptr, float *u_remapped = nullptr) const;

        size_t size() const
        {
            return bins.size();
        }

        const float PMF(int index) const 
        {
            return bins[index].p;
        }
    private:
        struct Bin
        {
            float q, p;
            int alias;
        };
        std::vector<Bin> bins;
    };
}