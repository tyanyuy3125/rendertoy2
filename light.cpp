#include <cmath>
#include <numeric>
#include <span>

#include "light.h"
#include "primitive.h"
#include "material.h"
#include "scene.h"
#include "texture.h"
#include "color.h"
#include "importer.h"

const glm::vec3 rendertoy::SurfaceLight::Sample_Ld(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, const bool consider_normal, bool &do_heuristic) const
{
    do_heuristic = true;
    glm::vec2 uv;
    glm::vec3 coord;
    glm::vec3 dir;
    glm::vec3 light_normal;
    _surface_primitive->GenerateSamplePointOnSurface(uv, coord, light_normal);
    dir = coord - intersect_info._coord;
    float projected_area = std::abs(glm::dot(light_normal, dir) * _surface_primitive->GetArea());
    if ((consider_normal && glm::dot(dir, intersect_info._geometry_normal) < 0.0f) || std::abs(projected_area) < 1e-4)
    {
        return glm::vec3(0.0f);
    }
    pdf = glm::dot(dir, dir) / projected_area;
    glm::vec3 normalized_dir = glm::normalize(dir);
    IntersectInfo shadow_ray_intersect_info;
    shadow_ray_intersect_info._time = intersect_info._time;
    scene.Intersect(intersect_info._coord, normalized_dir, shadow_ray_intersect_info);
    if (std::abs(shadow_ray_intersect_info._t - glm::length(dir)) > 1e-4f)
    {
        return glm::vec3(0.0f);
    }
    direction = glm::vec3(normalized_dir);
    // 重要：由于本项目采用了分层 BVH 结构，这里的 _material 可能不符合 _surface_primitive 存储的 _material。
    return _material->EvalEmissive(uv);
}

const glm::vec3 rendertoy::SurfaceLight::Sample_Ld(const Scene &scene, const glm::vec3 &view_point, glm::vec3 &direction, float &pdf, bool &do_heuristic) const
{
    do_heuristic = true;
    glm::vec2 uv;
    glm::vec3 coord;
    glm::vec3 dir;
    glm::vec3 light_normal;
    _surface_primitive->GenerateSamplePointOnSurface(uv, coord, light_normal);
    dir = coord - view_point;
    float projected_area = std::abs(glm::dot(light_normal, dir) * _surface_primitive->GetArea());
    if (std::abs(projected_area) < 1e-4f)
    {
        return glm::vec3(0.0f);
    }
    pdf = glm::dot(dir, dir) / projected_area;
    glm::vec3 normalized_dir = glm::normalize(dir);
    IntersectInfo shadow_ray_intersect_info; // TODO: 时间同步
    scene.Intersect(view_point, normalized_dir, shadow_ray_intersect_info);
    if (std::abs(shadow_ray_intersect_info._t - glm::length(dir)) > 1e-4f)
    {
        return glm::vec3(0.0f);
    }
    direction = glm::vec3(normalized_dir);
    return _material->EvalEmissive(uv);
}

const glm::vec3 rendertoy::SurfaceLight::Sample_Le(const glm::vec3 &last_origin, const IntersectInfo &intersect_info, float &pdf) const
{
    pdf = _surface_primitive->Pdf(intersect_info._coord - last_origin, intersect_info._uv);
    return _material->EvalEmissive(intersect_info._uv);
}

const float rendertoy::SurfaceLight::Phi() const
{
    // We assume all lights are two sided.
    return glm::two_pi<float>() * Luminance(glm::vec3(_material->albedo()->Avg())) * _material->strength()->Avg() * _surface_primitive->GetArea();
}

rendertoy::LightSampler::LightSampler(const std::vector<std::shared_ptr<Light>> &dls_lights)
{
    std::vector<float> light_power(dls_lights.size());
    for (size_t i = 0; i < dls_lights.size(); ++i)
    {
        light_power[i] = dls_lights[i]->Phi();
    }
    if (std::accumulate(light_power.begin(), light_power.end(), 0.f) == 0.f)
    {
        std::fill(light_power.begin(), light_power.end(), 1.f);
    }
    alias_table = AliasTable(light_power);
}

const glm::vec3 rendertoy::DeltaLight::Sample_Ld(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, const bool consider_normal, bool &do_heuristic) const
{
    do_heuristic = false;
    pdf = 1.0f;
    const glm::vec3 dir = _position - intersect_info._coord;
    // pdf = glm::dot(dir, dir);
    if ((consider_normal && glm::dot(dir, intersect_info._geometry_normal) < 0.0f))
    {
        return glm::vec3(0.0f);
    }
    const glm::vec3 normalized_dir = glm::normalize(dir);
    direction = normalized_dir;
    IntersectInfo shadow_ray_intersect_info;
    shadow_ray_intersect_info._time = intersect_info._time;
    bool intersected = scene.Intersect(intersect_info._coord, normalized_dir, shadow_ray_intersect_info);
    if (!intersected || (shadow_ray_intersect_info._t - glm::length(dir) > 1e-4f))
    {
        return _color * _strength / glm::dot(dir, dir);
    }
    return glm::vec3(0.0f);
}

const glm::vec3 rendertoy::DeltaLight::Sample_Ld(const Scene &scene, const glm::vec3 &view_point, glm::vec3 &direction, float &pdf, bool &do_heuristic) const
{
    do_heuristic = false;
    pdf = 1.0f;
    const glm::vec3 dir = _position - view_point;
    // pdf = glm::dot(dir, dir);
    const glm::vec3 normalized_dir = glm::normalize(dir);
    direction = normalized_dir;
    IntersectInfo shadow_ray_intersect_info;
    bool intersected = scene.Intersect(view_point, normalized_dir, shadow_ray_intersect_info);
    if (!intersected || (shadow_ray_intersect_info._t - glm::length(dir) > 1e-4f))
    {
        return _color * _strength / glm::dot(dir, dir);
    }
    return glm::vec3(0.0f);
}

const float rendertoy::DeltaLight::Phi() const
{
    return 4.0f * glm::pi<float>() * Luminance(_color) * _strength;
}

rendertoy::HDRILight::HDRILight(const std::string &path)
{
    Image raw_image(ImportImageFromFile(path));
    // while (true)
    // {
    //     if (raw_image.width() <= 512)
    //     {
    //         break;
    //     }
    //     raw_image = raw_image.NextMipMap();
    // }
    float weights[raw_image.width() * raw_image.height()];
    for (int v = 0; v < raw_image.height(); ++v)
    {
        float sinTheta = std::sin(glm::pi<float>() * float(v) / float(raw_image.height()));
        for (int u = 0; u < raw_image.width(); ++u)
        {
            weights[v * raw_image.width() + u] = Luminance(raw_image(u, v));
            weights[v * raw_image.width() + u] *= sinTheta;
        }
    }
    _hdri_map = std::make_shared<Image>(ImportImageFromFile(path));
    _distrib = std::make_shared<Distribution2D>(&weights[0], raw_image.width(), raw_image.height());
}

const glm::vec3 rendertoy::HDRILight::Sample_Ld(const Scene &scene, const IntersectInfo &intersect_info, float &pdf, glm::vec3 &direction, const bool consider_normal, bool &do_heuristic) const
{
    do_heuristic = true;

    float map_pdf;
    glm::vec2 uv = _distrib->SampleContinuous(glm::linearRand(glm::vec2(0.0f), glm::vec2(1.0f)), &map_pdf);
    if (map_pdf == 0.0f)
    {
        return glm::vec3(0.0f);
    }

    float theta = uv[1] * glm::pi<float>(), phi = uv[0] * glm::two_pi<float>();
    float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
    float sinPhi = std::sin(phi), cosPhi = std::cos(phi);

    direction = glm::vec3(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);

    pdf = map_pdf / (2 * glm::pi<float>() * glm::pi<float>() * sinTheta);
    if (sinTheta == 0)
        pdf = 0;

    if ((consider_normal && glm::dot(direction, intersect_info._geometry_normal) < 0.0f))
    {
        return glm::vec3(0.0f);
    }

    IntersectInfo shadow_ray_intersect_info;
    shadow_ray_intersect_info._time = intersect_info._time;
    bool intersected = scene.Intersect(intersect_info._coord, direction, shadow_ray_intersect_info);
    if (!intersected)
    {
        return _hdri_map->operator()(int(uv.x * _hdri_map->width()), int(uv.y * _hdri_map->height()));
    }
    return glm::vec3(0.0f);
}

const glm::vec3 rendertoy::HDRILight::Sample_Le(const glm::vec3 &last_origin, const IntersectInfo &intersect_info, float &pdf) const
{
    glm::vec3 w = -intersect_info._wo;
    glm::vec2 st(SphericalPhi(w) * glm::one_over_two_pi<float>(), SphericalTheta(w) * glm::one_over_pi<float>());
    return _hdri_map->operator()(int(st.x * _hdri_map->width()), int(st.y * _hdri_map->height()));
}
