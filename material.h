#pragma once

#include <memory>

#include "texture.h"

namespace rendertoy
{
    class IMaterial
    {
    protected:
        std::shared_ptr<ISamplable> _albedo;
    public:
        const std::shared_ptr<ISamplable> &albedo() const 
        {
            // TODO: nullptr
            return _albedo;
        }
        std::shared_ptr<ISamplable> &albedo()
        {
            return _albedo;
        }
        // virtual void Eval(float u, float v) const = 0;
    };

    class DiffuseBSDF : public IMaterial
    {
    public: // Temporal
        
    };

}