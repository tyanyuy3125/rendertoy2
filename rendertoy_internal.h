#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/norm.hpp>

#define RENDERTOY_FUNC_ARGUMENT_OUT &

template <typename T>
T RENDERTOY_DISCARD_VARIABLE;

#define BUILD_NUMBER 722
#define BUILD_DATE "2023-11-05+01:45:09"

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
    class Camera;
    class Emissive;
    class Image;
    class IMaterial;
    struct IntersectInfo;
    template <typename T = glm::vec4>
    class ISamplable;
    typedef ISamplable<glm::vec4> ISamplableColor;
    typedef ISamplable<float> ISamplableNumerical;
    class Light;
    class LightSampler;
    class Primitive;
    class Scene;
    class SurfaceLight;
    class TriangleMesh;

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

    inline const bool SameHemisphere(const glm::vec3 &w, const glm::vec3 &wp)
    {
        return w.z * wp.z > 0;
    }
}
