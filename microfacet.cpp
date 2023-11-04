#include "microfacet.h"

namespace rendertoy
{
    // The following microfacet utility code is borrowed from pbrt-v3.

    // Microfacet Utility Functions
    static void BeckmannSample11(float cosThetaI, float U1, float U2,
                                 float *slope_x, float *slope_y)
    {
        /* Special case (normal incidence) */
        if (cosThetaI > .9999)
        {
            float r = std::sqrt(-std::log(1.0f - U1));
            float sinPhi = std::sin(2 * glm::pi<float>() * U2);
            float cosPhi = std::cos(2 * glm::pi<float>() * U2);
            *slope_x = r * cosPhi;
            *slope_y = r * sinPhi;
            return;
        }

        /* The original inversion routine from the paper contained
           discontinuities, which causes issues for QMC integration
           and techniques like Kelemen-style MLT. The following code
           performs a numerical inversion with better behavior */
        float sinThetaI =
            std::sqrt(std::max((float)0, (float)1 - cosThetaI * cosThetaI));
        float tanThetaI = sinThetaI / cosThetaI;
        float cotThetaI = 1 / tanThetaI;

        /* Search interval -- everything is parameterized
           in the Erf() domain */
        float a = -1, c = Erf(cotThetaI);
        float sample_x = std::max(U1, (float)1e-6f);

        /* Start with a good initial guess */
        // float b = (1-sample_x) * a + sample_x * c;

        /* We can do better (inverse of an approximation computed in
         * Mathematica) */
        float thetaI = std::acos(cosThetaI);
        float fit = 1 + thetaI * (-0.876f + thetaI * (0.4265f - 0.0594f * thetaI));
        float b = c - (1 + c) * std::pow(1 - sample_x, fit);

        /* Normalization factor for the CDF */
        static const float SQRT_PI_INV = 1.f / std::sqrt(glm::pi<float>());
        float normalization =
            1 /
            (1 + c + SQRT_PI_INV * tanThetaI * std::exp(-cotThetaI * cotThetaI));

        int it = 0;
        while (++it < 10)
        {
            /* Bisection criterion -- the oddly-looking
               Boolean expression are intentional to check
               for NaNs at little additional cost */
            if (!(b >= a && b <= c))
                b = 0.5f * (a + c);

            /* Evaluate the CDF and its derivative
               (i.e. the density function) */
            float invErf = ErfInv(b);
            float value =
                normalization *
                    (1 + b + SQRT_PI_INV * tanThetaI * std::exp(-invErf * invErf)) -
                sample_x;
            float derivative = normalization * (1 - invErf * tanThetaI);

            if (std::abs(value) < 1e-5f)
                break;

            /* Update bisection intervals */
            if (value > 0)
                c = b;
            else
                a = b;

            b -= value / derivative;
        }

        /* Now convert back into a slope value */
        *slope_x = ErfInv(b);

        /* Simulate Y component */
        *slope_y = ErfInv(2.0f * std::max(U2, (float)1e-6f) - 1.0f);
    }

    static glm::vec3 BeckmannSample(const glm::vec3 &wi, float alpha_x, float alpha_y,
                                    float U1, float U2)
    {
        // 1. stretch wi
        glm::vec3 wiStretched =
            glm::normalize(glm::vec3(alpha_x * wi.x, alpha_y * wi.y, wi.z));

        // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
        float slope_x, slope_y;
        BeckmannSample11(CosTheta(wiStretched), U1, U2, &slope_x, &slope_y);

        // 3. rotate
        float tmp = CosPhi(wiStretched) * slope_x - SinPhi(wiStretched) * slope_y;
        slope_y = SinPhi(wiStretched) * slope_x + CosPhi(wiStretched) * slope_y;
        slope_x = tmp;

        // 4. unstretch
        slope_x = alpha_x * slope_x;
        slope_y = alpha_y * slope_y;

        // 5. compute normal
        return glm::normalize(glm::vec3(-slope_x, -slope_y, 1.f));
    }
}

const float rendertoy::BeckmannDistribution::D(const glm::vec3 &wh) const
{
    float tan2Theta = Tan2Theta(wh);
    if (std::isinf(tan2Theta))
        return 0.;
    float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
    return std::exp(-tan2Theta * (Cos2Phi(wh) / (alphax * alphax) +
                                  Sin2Phi(wh) / (alphay * alphay))) /
           (glm::pi<float>() * alphax * alphay * cos4Theta);
}

const float rendertoy::BeckmannDistribution::Lambda(const glm::vec3 &w) const
{
    float absTanTheta = std::abs(TanTheta(w));
    if (std::isinf(absTanTheta))
        return 0.0f;
    // Compute _alpha_ for direction _w_
    float alpha =
        std::sqrt(Cos2Phi(w) * alphax * alphax + Sin2Phi(w) * alphay * alphay);
    float a = 1.0f / (alpha * absTanTheta);
    if (a >= 1.6f)
        return 0.0f;
    return (1.0f - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

const glm::vec3 rendertoy::BeckmannDistribution::Sample_wh(const glm::vec3 &wo,
                                         const glm::vec2 &u) const {
    if (!sampleVisibleArea) {
        // Sample full distribution of normals for Beckmann distribution

        // Compute $\tan^2 \theta$ and $\phi$ for Beckmann distribution sample
        float tan2Theta, phi;
        if (alphax == alphay) {
            float logSample = std::log(1 - u[0]);
            tan2Theta = -alphax * alphax * logSample;
            phi = u[1] * 2.0f * glm::pi<float>();
        } else {
            // Compute _tan2Theta_ and _phi_ for anisotropic Beckmann
            // distribution
            float logSample = std::log(1 - u[0]);
            phi = std::atan(alphay / alphax *
                            std::tan(2 * glm::pi<float>() * u[1] + 0.5f * glm::pi<float>()));
            if (u[1] > 0.5f) phi += glm::pi<float>();
            float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
            float alphax2 = alphax * alphax, alphay2 = alphay * alphay;
            tan2Theta = -logSample /
                        (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
        }

        // Map sampled Beckmann angles to normal direction _wh_
        float cosTheta = 1 / std::sqrt(1 + tan2Theta);
        float sinTheta = std::sqrt(std::max((float)0, 1 - cosTheta * cosTheta));
        glm::vec3 wh = SphericalDirection(sinTheta, cosTheta, phi);
        if (!SameHemisphere(wo, wh)) wh = -wh;
        return wh;
    } else {
        // Sample visible area of normals for Beckmann distribution
        glm::vec3 wh;
        bool flip = wo.z < 0;
        wh = BeckmannSample(flip ? -wo : wo, alphax, alphay, u[0], u[1]);
        if (flip) wh = -wh;
        return wh;
    }
}

