#pragma once

#include <memory>

#include "texture.h"

#define MATERIAL_SOCKET(name, isamplable_type) \
protected:\
std::shared_ptr<ISamplable##isamplable_type> _##name = default_##name;\
public:\
const std::shared_ptr<ISamplable##isamplable_type>& name() const\
{\
    return _##name;\
}\
std::shared_ptr<ISamplable##isamplable_type>& name()\
{\
    return _##name;\
}\
private:\

namespace rendertoy
{
    static std::shared_ptr<ISamplableColor> default_albedo = std::make_shared<ColorTexture>(glm::vec4(1.0f));
    static std::shared_ptr<ISamplableNumerical> default_strength = std::make_shared<ConstantNumerical>(1.0f);

    class IMaterial
    {
        MATERIAL_SOCKET(albedo, Color);
    public:
        IMaterial(const std::shared_ptr<ISamplableColor> &albedo = default_albedo)
        : _albedo(albedo) {}
        /// @brief 
        /// @param uv 
        /// @note Plane light should not be IES-like, so taking uv is enough.
        /// @return 
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const = 0;
        /// @brief 
        /// @param intersect_info 
        /// @param out 
        /// @return BSDF.
        virtual const glm::vec3 Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const = 0;
        /// @brief Generates new sample light from the intersection point, and computes corresponding PDF & BSDF.
        /// @param intersect_info 
        /// @param pdf 
        /// @param bsdf 
        /// @return Direction of new sample light.
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf) const = 0;
    };

    class DiffuseBSDF : public IMaterial
    {
    public:
        DiffuseBSDF(const std::shared_ptr<ISamplableColor> &albedo = default_albedo)
        : IMaterial(albedo) {}
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const;
        virtual const glm::vec3 Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const;
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf) const;
    };

    class Emissive : public IMaterial
    {
        MATERIAL_SOCKET(strength, Numerical);
    public:
        Emissive(const std::shared_ptr<ISamplableColor> &albedo = default_albedo, const std::shared_ptr<ISamplableNumerical> &strength = default_strength)
        : IMaterial(albedo), _strength(strength) {}
        virtual const glm::vec3 EvalEmissive(const glm::vec2 &uv) const;
        virtual const glm::vec3 Eval(const IntersectInfo &intersect_info, const glm::vec3 &out, float &pdf) const;
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &bsdf) const;
    };

}