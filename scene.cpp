#include <memory>
#include <stack>

#include "scene.h"
#include "material.h"
#include "primitive.h"
#include "light.h"
#include "texture.h"

void rendertoy::Scene::Init()
{
    _objects.Construct();
    _dls_lights.clear();
    for(const auto &light : _lights)
    {
        _dls_lights.push_back(light);
    }
    for (const std::shared_ptr<Primitive> object : _objects.objects)
    {
        std::shared_ptr<Emissive> emissive_mat = std::dynamic_pointer_cast<Emissive>(object->_mat);
        if (!emissive_mat)
        {
            continue;
        }

        if (object->PRIMITIVE_TYPE() == FUNDAMENTAL_PRIMITIVE)
        {
            _dls_lights.push_back(std::make_shared<SurfaceLight>(object, emissive_mat));
            object->_surface_light = (SurfaceLight *)(_dls_lights[_dls_lights.size() - 1].get());
        }
        else // COMBINED_PRIMITIVE
        {
            std::shared_ptr<TriangleMesh> triangle_mesh = std::dynamic_pointer_cast<TriangleMesh>(object);
            if (triangle_mesh)
            {
                for (const std::shared_ptr<Triangle> &triangle : triangle_mesh->triangles())
                {
                    _dls_lights.push_back(std::make_shared<SurfaceLight>(triangle, emissive_mat));
                    triangle->_surface_light = (SurfaceLight *)(_dls_lights[_dls_lights.size() - 1].get());
                }
            }
        }
    }
    _light_sampler = std::make_shared<LightSampler>(_dls_lights);
}

const bool rendertoy::Scene::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
{
#define ALPHA_TEST
#ifdef ALPHA_TEST
    bool ret = _objects.Intersect(origin, direction, intersect_info);
    if(!ret)
    {
        return ret;
    }
    else
    {
        float alpha = intersect_info._mat->albedo()->Sample(intersect_info._uv).w;
        if(glm::linearRand(0.0f, ONE_MINUS_EPSILON) > alpha)
        {
            return Intersect(intersect_info._coord, direction, intersect_info);
        }
        else
        {
            return true;
        }
    }
#else
    return _objects.Intersect(origin, direction, intersect_info);
#endif
}

const glm::vec3 rendertoy::Scene::SampleLights(const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, SurfaceLight *&sampled_light, const bool consider_normal) const
{
    if(_dls_lights.size() == 0)
    {
        return glm::vec3(0.0f);
    }
    float pmf;
// #define DISABLE_POWER_LIGHT_SAMPLER
#ifdef DISABLE_POWER_LIGHT_SAMPLER
    pmf = 1.0f / _dls_lights.size();
    int idx = glm::linearRand<int>(0, static_cast<int>(_dls_lights.size()) - 1);
#else
    int idx = _light_sampler->Sample(&pmf);
#endif // DISABLE_POWER_LIGHT_SAMPLER
    sampled_light = (SurfaceLight *)_dls_lights[idx].get();
    return (1.0f / pmf) * _dls_lights[idx]->Sample_Ld(*this, intersect_info, pdf, direction, consider_normal);
}
