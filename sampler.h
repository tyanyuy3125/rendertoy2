#pragma once

#include <vector>
#include <span>

#include "rendertoy_internal.h"

namespace rendertoy
{
    const glm::vec3 UniformSampleHemisphere();
    const float UniformSampleHemispherePdf();
    const glm::vec2 ConcentricSampleDisk();
    const glm::vec3 CosineSampleHemisphere();
    const float CosineSampleHemispherePdf(float cosTheta);
    const float PowerHeuristic(int nf, float f_pdf, int ng, float g_pdf);
    const glm::vec3 UniformSampleSphere();
    const float SampleExponential(const float u, const float a);
    const int SampleDiscrete(std::span<const float> weights);

    struct Distribution1D
    {
        Distribution1D(const float *f, int n) : func(f, f + n), cdf(n + 1)
        {
            cdf[0] = 0;
            for (int i = 1; i < n + 1; ++i)
                cdf[i] = cdf[i - 1] + func[i - 1] / n;

            funcInt = cdf[n];
            if (funcInt == 0)
            {
                for (int i = 1; i < n + 1; ++i)
                    cdf[i] = float(i) / float(n);
            }
            else
            {
                for (int i = 1; i < n + 1; ++i)
                    cdf[i] /= funcInt;
            }
        }
        int Count() const { return (int)func.size(); }
        float SampleContinuous(float u, float *pdf, int *off = nullptr) const
        {
            int offset = FindInterval((int)cdf.size(),
                                      [&](int index)
                                      { return cdf[index] <= u; });
            if (off)
                *off = offset;
            float du = u - cdf[offset];
            if ((cdf[offset + 1] - cdf[offset]) > 0)
            {
                du /= (cdf[offset + 1] - cdf[offset]);
            }

            if (pdf)
                *pdf = (funcInt > 0) ? func[offset] / funcInt : 0;

            return (offset + du) / Count();
        }
        int SampleDiscrete(float u, float *pdf = nullptr,
                           float *uRemapped = nullptr) const
        {
            int offset = FindInterval((int)cdf.size(),
                                      [&](int index)
                                      { return cdf[index] <= u; });
            if (pdf)
                *pdf = (funcInt > 0) ? func[offset] / (funcInt * Count()) : 0;
            if (uRemapped)
                *uRemapped = (u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
            return offset;
        }
        float DiscretePDF(int index) const
        {
            return func[index] / (funcInt * Count());
        }

        std::vector<float> func, cdf;
        float funcInt;
    };

    class Distribution2D
    {
    public:
        Distribution2D(const float *data, int nu, int nv);
        glm::vec2 SampleContinuous(const glm::vec2 &u, float *pdf) const
        {
            float pdfs[2];
            int v;
            float d1 = pMarginal->SampleContinuous(u[1], &pdfs[1], &v);
            float d0 = pConditionalV[v]->SampleContinuous(u[0], &pdfs[0]);
            *pdf = pdfs[0] * pdfs[1];
            return glm::vec2(d0, d1);
        }
        float Pdf(const glm::vec2 &p) const
        {
            int iu = glm::clamp(int(p[0] * pConditionalV[0]->Count()), 0,
                           pConditionalV[0]->Count() - 1);
            int iv =
                glm::clamp(int(p[1] * pMarginal->Count()), 0, pMarginal->Count() - 1);
            return pConditionalV[iv]->func[iu] / pMarginal->funcInt;
        }

    private:
        std::vector<std::unique_ptr<Distribution1D>> pConditionalV;
        std::unique_ptr<Distribution1D> pMarginal;
    };

    class AliasTable
    {
    public:
        AliasTable() {} // Undefined Behaviour.
        AliasTable(std::span<float> weights);

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