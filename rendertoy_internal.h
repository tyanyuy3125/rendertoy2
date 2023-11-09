#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define RENDERTOY_FUNC_ARGUMENT_OUT &

template <typename T>
T RENDERTOY_DISCARD_VARIABLE;

#define BUILD_NUMBER 1336
#define BUILD_DATE "2023-11-10+00:54:06"

#define CLASS_METADATA_MARK(classname)                              \
public:                                                             \
    virtual const char *GetClassName() const { return #classname; } \
                                                                    \
private:

#define MATERIAL_SOCKET(name, isamplable_type)                       \
protected:                                                           \
    std::shared_ptr<ISamplable##isamplable_type> _##name;            \
                                                                     \
public:                                                              \
    const std::shared_ptr<ISamplable##isamplable_type> &name() const \
    {                                                                \
        return _##name;                                              \
    }                                                                \
    std::shared_ptr<ISamplable##isamplable_type> &name()             \
    {                                                                \
        return _##name;                                              \
    }                                                                \
                                                                     \
private:

#define ONE_MINUS_EPSILON float(0x1.fffffep-1)

#define FUNDAMENTAL_PRIMITIVE 0x01
#define COMBINED_PRIMITIVE 0x02

namespace rendertoy
{
    // Forward declarations
    class AliasTable;
    class BeckmannDistribution;
    class BSDF;
    class BxDF;
    class Camera;
    class DeltaLight;
    class Distribution1D;
    class Distribution2D;
    class Emissive;
    class Fresnel;
    class FresnelConductor;
    class FresnelDielectric;
    class FresnelNoOp;
    class Image;
    class IMaterial;
    struct IntersectInfo;
    template <typename T = glm::vec4>
    class ISamplable;
    typedef ISamplable<glm::vec4> ISamplableColor;
    typedef ISamplable<float> ISamplableNumerical;
    class IsotropicPhaseFunction;
    class HenyeyGreensteinPhaseFunction;
    class LambertianReflection;
    class Light;
    class LightSampler;
    class MicrofacetDistribution;
    class MicrofacetReflection;
    class OrenNayer;
    class PhaseFunction;
    class Primitive;
    class Scene;
    class SurfaceLight;
    class SpecularReflection;
    class TriangleMesh;
    struct VolumeInteraction;

    inline float CosTheta(const glm::vec3 &w) { return w.z; }
    inline float Cos2Theta(const glm::vec3 &w) { return w.z * w.z; }
    inline float AbsCosTheta(const glm::vec3 &w) { return std::abs(w.z); }
    inline float Sin2Theta(const glm::vec3 &w)
    {
        return std::max(0.0f, 1.0f - Cos2Theta(w));
    }
    inline float SinTheta(const glm::vec3 &w)
    {
        return std::sqrt(Sin2Theta(w));
    }
    inline float TanTheta(const glm::vec3 &w)
    {
        return SinTheta(w) / CosTheta(w);
    }
    inline float Tan2Theta(const glm::vec3 &w)
    {
        return Sin2Theta(w) / Cos2Theta(w);
    }
    inline float CosPhi(const glm::vec3 &w)
    {
        float sinTheta = SinTheta(w);
        return (sinTheta == 0) ? 1 : glm::clamp(w.x / sinTheta, -1.0f, 1.0f);
    }
    inline float SinPhi(const glm::vec3 &w)
    {
        float sinTheta = SinTheta(w);
        return (sinTheta == 0) ? 0 : glm::clamp(w.y / sinTheta, -1.0f, 1.0f);
    }
    inline float Cos2Phi(const glm::vec3 &w)
    {
        return CosPhi(w) * CosPhi(w);
    }
    inline float Sin2Phi(const glm::vec3 &w)
    {
        return SinPhi(w) * SinPhi(w);
    }
    inline float CosDPhi(const glm::vec3 &wa, const glm::vec3 &wb)
    {
        return glm::clamp((wa.x * wb.x + wa.y * wb.y) /
                              std::sqrt((wa.x * wa.x + wa.y * wa.y) *
                                        (wb.x * wb.x + wb.y * wb.y)),
                          -1.0f, 1.0f);
    }
    inline float AbsDot(const glm::vec3 &a, const glm::vec3 &b)
    {
        return std::abs(glm::dot(a, b));
    }
    inline float Erf(float x)
    {
        // constants
        float a1 = 0.254829592f;
        float a2 = -0.284496736f;
        float a3 = 1.421413741f;
        float a4 = -1.453152027f;
        float a5 = 1.061405429f;
        float p = 0.3275911f;

        // Save the sign of x
        int sign = 1;
        if (x < 0)
            sign = -1;
        x = std::abs(x);

        // A&S formula 7.1.26
        float t = 1 / (1 + p * x);
        float y =
            1 -
            (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

        return sign * y;
    }
    inline float ErfInv(float x)
    {
        float w, p;
        x = glm::clamp(x, -.99999f, .99999f);
        w = -std::log((1 - x) * (1 + x));
        if (w < 5)
        {
            w = w - 2.5f;
            p = 2.81022636e-08f;
            p = 3.43273939e-07f + p * w;
            p = -3.5233877e-06f + p * w;
            p = -4.39150654e-06f + p * w;
            p = 0.00021858087f + p * w;
            p = -0.00125372503f + p * w;
            p = -0.00417768164f + p * w;
            p = 0.246640727f + p * w;
            p = 1.50140941f + p * w;
        }
        else
        {
            w = std::sqrt(w) - 3;
            p = -0.000200214257f;
            p = 0.000100950558f + p * w;
            p = 0.00134934322f + p * w;
            p = -0.00367342844f + p * w;
            p = 0.00573950773f + p * w;
            p = -0.0076224613f + p * w;
            p = 0.00943887047f + p * w;
            p = 1.00167406f + p * w;
            p = 2.83297682f + p * w;
        }
        return p * x;
    }
    inline const glm::vec3 SphericalDirection(const float sinTheta, const float cosTheta, const float phi)
    {
        return glm::vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi),
                         cosTheta);
    }

    inline const glm::vec3 SphericalDirection(float sinTheta, float cosTheta, float phi,
                                              const glm::vec3 &x, const glm::vec3 &y,
                                              const glm::vec3 &z)
    {
        return sinTheta * std::cos(phi) * x + sinTheta * std::sin(phi) * y +
               cosTheta * z;
    }

    inline const bool SameHemisphere(const glm::vec3 &w, const glm::vec3 &wp)
    {
        return w.z * wp.z > 0;
    }

    inline glm::vec3 Faceforward(const glm::vec3 &v, const glm::vec3 &v2)
    {
        return (glm::dot(v, v2) < 0.f) ? -v : v;
    }

    inline glm::vec3 Reflect(const glm::vec3 &wo, const glm::vec3 &n)
    {
        return -wo + 2 * glm::dot(wo, n) * n;
    }

    inline bool Refract(const glm::vec3 &wi, const glm::vec3 &n, float eta,
                        glm::vec3 *wt)
    {
        // Compute $\cos \theta_\roman{t}$ using Snell's law
        float cosThetaI = glm::dot(n, wi);
        float sin2ThetaI = std::max(0.0f, 1.0f - cosThetaI * cosThetaI);
        float sin2ThetaT = eta * eta * sin2ThetaI;

        // Handle total internal reflection for transmission
        if (sin2ThetaT >= 1.0f)
            return false;
        float cosThetaT = std::sqrt(1.0f - sin2ThetaT);
        *wt = eta * -wi + (eta * cosThetaI - cosThetaT) * glm::vec3(n);
        return true;
    }

    inline void CoordinateSystem(const glm::vec3 &v1, glm::vec3 *v2,
                                 glm::vec3 *v3)
    {
        if (std::abs(v1.x) > std::abs(v1.y))
            *v2 = glm::vec3(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
        else
            *v2 = glm::vec3(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
        *v3 = glm::cross(v1, *v2);
    }

    template <typename Predicate>
    int FindInterval(int size, const Predicate &pred)
    {
        int first = 0, len = size;
        while (len > 0)
        {
            int half = len >> 1, middle = first + half;
            if (pred(middle))
            {
                first = middle + 1;
                len -= half + 1;
            }
            else
                len = half;
        }
        return glm::clamp(first - 1, 0, size - 2);
    }

    inline float SphericalTheta(const glm::vec3 &v)
    {
        return std::acos(glm::clamp(v.y, -1.0f, 1.0f));
    }

    inline float SphericalPhi(const glm::vec3 &v)
    {
        float p = std::atan2(v.z, v.x);
        return (p < 0) ? (p + 2 * glm::pi<float>()) : p;
    }

}
