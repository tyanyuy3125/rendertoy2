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
        virtual const glm::vec3 Eval(const IntersectInfo &) const = 0;
        virtual const glm::vec3 Sample(const IntersectInfo &) const = 0;
    };

    class DiffuseBSDF : public IMaterial
    {
    public:
        DiffuseBSDF(const std::shared_ptr<ISamplableColor> &albedo = default_albedo)
        : IMaterial(albedo) {}

        virtual const glm::vec3 Eval(const IntersectInfo &intersect_info) const;
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info) const;
    };

    class Emissive : public IMaterial
    {
        MATERIAL_SOCKET(strength, Numerical);
    public:
        Emissive(const std::shared_ptr<ISamplableColor> &albedo = default_albedo)
        : IMaterial(albedo) {}

        virtual const glm::vec3 Eval(const IntersectInfo &intersect_info) const;
        virtual const glm::vec3 Sample(const IntersectInfo &intersect_info) const;
    };

}