#include <glm/gtc/random.hpp>
#include <memory>

#include "scene.h"
#include "material.h"

void rendertoy::Scene::Init()
{
    _objects.Construct();
    _dls_lights.clear();
    // Update _dls_lights for Direct Light Sampling.
    for (const std::shared_ptr<Primitive> object : _objects.objects)
    {
        // TODO: 以下代码存在bug，例如对于设置为发光的 TriangleMesh，其中所有的多边形都会被强制设置为发光。
        // REMARK: 建议在必要时对材质赋予系统进行完全重构！
        if (typeid(*(object.get())) == typeid(TriangleMesh) && typeid(*(object->mat().get())) == typeid(Emissive))
        {
            for (const std::shared_ptr<Primitive> triangle : dynamic_cast<TriangleMesh *>(object.get())->triangles())
            {
                _dls_lights.push_back(std::make_shared<SurfaceLight>(triangle, object->mat()));
                object->_surface_light = (SurfaceLight *)(_dls_lights[_dls_lights.size() - 1].get());
            }
        }
        else if (typeid(*(object.get())) == typeid(TriangleMesh))
        {
            for (const std::shared_ptr<Primitive> triangle : dynamic_cast<TriangleMesh *>(object.get())->triangles())
            {
                if (!object->mat().get() && typeid(*(triangle->mat().get())) == typeid(Emissive))
                {
                    _dls_lights.push_back(std::make_shared<SurfaceLight>(triangle, triangle->mat()));
                    object->_surface_light = (SurfaceLight *)(_dls_lights[_dls_lights.size() - 1].get());
                }
            }
        }
        else
        {
            if (typeid(*(object->mat().get())) == typeid(Emissive))
            {
                _dls_lights.push_back(std::make_shared<SurfaceLight>(object, object->mat()));
                object->_surface_light = (SurfaceLight *)(_dls_lights[_dls_lights.size() - 1].get());
            }
        }
    }
}

const bool rendertoy::Scene::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
{
    return _objects.Intersect(origin, direction, intersect_info);
}

const glm::vec3 rendertoy::Scene::SampleLights(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, SurfaceLight *&sampled_light) const
{
    int idx = glm::linearRand<int>(0, static_cast<int>(_dls_lights.size()) - 1);
    sampled_light = (SurfaceLight *)_dls_lights[idx].get();
    return _dls_lights[idx]->Sample(*this, intersect_info, pdf, direction);
}
