#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/norm.hpp>

#define RENDERTOY_FUNC_ARGUMENT_OUT &

template <typename T>
T RENDERTOY_DISCARD_VARIABLE;

#define BUILD_NUMBER 658
#define BUILD_DATE "2023-11-05+01:26:09"

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
}
