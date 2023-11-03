#include <cmath>

#include "light.h"
#include "primitive.h"
#include "material.h"
#include "scene.h"

const glm::vec3 rendertoy::SurfaceLight::Sample(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction) const
{
    glm::vec2 uv;
    glm::vec3 coord;
    glm::vec3 dir;
    glm::vec3 light_normal;
    _surface_primitive->GenerateSamplePointOnSurface(uv, coord, light_normal);
    dir = coord - intersect_info._coord;
    float projected_area = std::abs(glm::dot(light_normal, dir) * _surface_primitive->GetArea());
    if(glm::dot(dir, intersect_info._normal) < 0.0f || std::abs(projected_area) < 1e-4)
    {
        return glm::vec3(0.0f);
    }
    pdf = glm::dot(dir, dir) / projected_area;
    glm::vec3 normalized_dir = glm::normalize(dir);
    IntersectInfo shadow_ray_intersect_info;
    scene.Intersect(intersect_info._coord, normalized_dir, shadow_ray_intersect_info);
    if(std::abs(shadow_ray_intersect_info._t - glm::length(dir)) > 1e-4)
    {
        return glm::vec3(0.0f);
    }
    direction = glm::vec3(normalized_dir);
    // 重要：由于本项目采用了分层 BVH 结构，这里的 _material 可能不符合 _surface_primitive 存储的 _material。
    return _material->EvalEmissive(uv);
}