#pragma once

#include <memory>

#include "texture.h"

namespace rendertoy
{
    static std::shared_ptr<ISamplable> default_albedo = std::make_shared<ColorTexture>(glm::vec4(1.0f));

    class IMaterial
    {
    protected:
        std::shared_ptr<ISamplable> _albedo = default_albedo;
    public:
        const std::shared_ptr<ISamplable> &albedo() const 
        {
            // _albedo should not be nullptr unless a programmer intentially sets that.
            return _albedo;
        }
        std::shared_ptr<ISamplable> &albedo()
        {
            return _albedo;
        }

        IMaterial(const std::shared_ptr<ISamplable> &albedo = default_albedo)
        : _albedo(albedo) {}
        // virtual void Eval(float u, float v) const = 0;
    };

    class DiffuseBSDF : public IMaterial
    {
    public:
        DiffuseBSDF(const std::shared_ptr<ISamplable> &albedo = default_albedo)
        : IMaterial(albedo) {};
    };

}