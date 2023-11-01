#pragma once

#include <memory>
#include <vector>

#include "accelerate.h"
#include "primitive.h"
#include "accelerate.h"
#include "texture.h"
#include "light.h"

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
    static std::shared_ptr<ISamplableColor> default_hdr_background = std::make_shared<ColorTexture>(glm::vec4(0.1f));

    class Scene
    {
    private:
        BVH<Primitive> _objects;
        std::vector<std::shared_ptr<Light>> _dls_lights;
        std::vector<std::shared_ptr<Light>> _lights;

        MATERIAL_SOCKET(hdr_background, Color);
    public:
        Scene() = default;
        Scene(const Scene &) = delete;

        const std::vector<std::shared_ptr<Primitive>> &objects() const
        {
            return _objects.objects;
        }
        std::vector<std::shared_ptr<Primitive>> &objects()
        {
            return _objects.objects;
        }

        void Init();
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const;
        const glm::vec3 SampleLights(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction) const;
    };
}
