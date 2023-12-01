#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "primitive.h"
#include "logger.h"
#include "material.h"
#include "texture.h"

void rendertoy::TriangleMesh::GetCurrentAnimationState(const float time, glm::quat &rot, glm::vec3 &tran) const
{
    float time_factor = (time - _time_from) / (_time_to - _time_from);
    rot = glm::slerp(_rot_from, _rot_to, time_factor);
    tran = glm::mix(_tran_from, _tran_to, time_factor);
}

const bool rendertoy::TriangleMesh::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo &intersect_info) const
{
    bool ret;
    if (_is_animated)
    {
        glm::quat rot;
        glm::vec3 tran;
        GetCurrentAnimationState(intersect_info._time, rot, tran);
        glm::mat3 _rot = glm::toMat3(rot);
        glm::vec3 local_origin = glm::transpose(_rot) * origin - glm::transpose(_rot) * tran;
        glm::vec3 local_dir = glm::transpose(_rot) * direction;
        ret = _triangles.Intersect(local_origin, local_dir, intersect_info);
    }
    else
    {
        ret = _triangles.Intersect(origin, direction, intersect_info);
    }
    if (intersect_info._mat == nullptr)
    {
        intersect_info._mat = _mat;
    }
    if (_mat->bump())
    {
        intersect_info._shading_normal += glm::vec3(_mat->bump()->Sample(intersect_info._uv));
        intersect_info._shading_normal = glm::normalize(intersect_info._shading_normal);
    }
    return ret;
}

void rendertoy::TriangleMesh::Animate(const glm::quat &rot_to, const glm::vec3 &tran_to, const glm::float32 time_from, const glm::float32 time_to)
{
    _rot_to = rot_to;
    _tran_to = tran_to;
    _time_from = time_from;
    _time_to = time_to;
    _is_animated = true;

    // Expand current BVH to contain any rotations
    float radius = glm::length(_bbox.Diagonal()) / 2.0f;
    glm::vec3 new_pmin = _bbox.GetCenter() - glm::vec3(radius);
    glm::vec3 new_pmax = _bbox.GetCenter() + glm::vec3(radius);
    _bbox = BBox(new_pmin, new_pmax);

    // Translate the BBox
    glm::vec3 new_new_pmin = new_pmin + tran_to;
    glm::vec3 new_new_pmax = new_pmax + tran_to;

    // Union two BBox
    _bbox.Union(BBox(new_new_pmin, new_new_pmax));
}

const rendertoy::BBox rendertoy::TriangleMesh::GetBoundingBox() const
{
    return _bbox;
}

const glm::vec3 rendertoy::TriangleMesh::GetCenter() const
{
    return _bbox.GetCenter();
}

const float rendertoy::TriangleMesh::GetArea() const
{
    CRIT << "Not implemented." << std::endl;
    return 0.0f;
}

const void rendertoy::TriangleMesh::GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const
{
    int idx = glm::linearRand<int>(0, static_cast<int>(this->triangles().size()) - 1);
    this->triangles()[idx]->GenerateSamplePointOnSurface(uv, coord, normal);
}

const bool rendertoy::Triangle::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
{
    glm::vec3 v0v1 = _vert[1] - _vert[0];
    glm::vec3 v0v2 = _vert[2] - _vert[0];
    glm::vec3 pvec = glm::cross(direction, v0v2);
    glm::float32 det = glm::dot(v0v1, pvec);
    if (std::abs(det) < 1e-6f)
        return false;
    glm::float32 invDet = 1.0f / det;
    glm::vec3 tvec = origin - _vert[0];
    glm::float32 u = glm::dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
        return false;
    glm::vec3 qvec = glm::cross(tvec, v0v1);
    glm::float32 v = glm::dot(direction, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f)
        return false;
    glm::float32 t = glm::dot(v0v2, qvec) * invDet;
    if (t < 1e-3f)
        return false;
    intersect_info._uv = u * _uv[1] + v * _uv[2] + (1 - u - v) * _uv[0];
    intersect_info._coord = origin + t * direction;
    intersect_info._t = t;
    // intersect_info._geometry_normal = glm::normalize(glm::cross(v0v1, v0v2));
    intersect_info._geometry_normal = u * _norm[1] + v * _norm[2] + (1 - u - v) * _norm[0];
    intersect_info._shading_normal = intersect_info._geometry_normal;
    intersect_info._mat = _mat;
    intersect_info._wo = -direction;
    intersect_info._primitive = (Primitive *)this;
    if (glm::dot(intersect_info._geometry_normal, direction) > 0.0f)
    {
        intersect_info._geometry_normal = -intersect_info._geometry_normal;
    }
    return true;
}

const rendertoy::BBox rendertoy::Triangle::GetBoundingBox() const
{
    glm::vec3 pmin = _vert[0], pmax = _vert[0];
    for (int i = 1; i < 3; ++i)
    {
        pmin.x = std::min(_vert[i].x, pmin.x);
        pmin.y = std::min(_vert[i].y, pmin.y);
        pmin.z = std::min(_vert[i].z, pmin.z);

        pmax.x = std::max(_vert[i].x, pmax.x);
        pmax.y = std::max(_vert[i].y, pmax.y);
        pmax.z = std::max(_vert[i].z, pmax.z);
    }
    return BBox{pmin, pmax};
}

const float rendertoy::Triangle::GetArea() const
{
    return glm::length(glm::cross(_vert[1] - _vert[0], _vert[2] - _vert[0])) / 2.0f;
}

const rendertoy::SurfaceLight *rendertoy::Triangle::GetSurfaceLight() const
{
    return _surface_light;
}

const void rendertoy::Triangle::GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const
{
    float u = glm::linearRand<float>(0.0f, 1.0f);
    float v = glm::linearRand<float>(0.0f, 1.0f);
    if (u + v > 1.0f)
    {
        u = 1.0f - u;
        v = 1.0f - v;
    }
    coord = u * _vert[1] + v * _vert[2] + (1.0f - u - v) * _vert[0];
    uv = glm::vec2(u, v);
    normal = u * _norm[1] + v * _norm[2] + (1.0f - u - v) * _norm[0];
}

const float rendertoy::Triangle::Pdf(const glm::vec3 &observation_to_primitive, const glm::vec2 &uv) const
{
    float projected_area = std::abs(glm::dot(this->GetNormal(uv), glm::normalize(observation_to_primitive))) * GetArea();
    if (std::abs(projected_area) < 1e-4)
    {
        return 0.0f;
    }
    return glm::dot(observation_to_primitive, observation_to_primitive) / projected_area;
}

const glm::vec3 rendertoy::Triangle::GetNormal(const glm::vec2 &uv) const
{
    return uv.x * _norm[1] + uv.y * _norm[2] + (1.0f - uv.x - uv.y) * _norm[0];
}

const rendertoy::SurfaceLight *rendertoy::Primitive::GetSurfaceLight() const
{
    return nullptr;
}

const float rendertoy::Primitive::Pdf(const glm::vec3 &observation_to_primitive, const glm::vec2 &uv) const
{
    return 0.0f;
}

const glm::vec3 rendertoy::Primitive::GetNormal(const glm::vec2 &uv) const
{
    return glm::vec3(0.0f);
}

const bool rendertoy::SDF::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo &intersect_info) const
{
    // Sphere tracing algorithm

    float tmin, tmax;
    if(!this->_bbox.Intersect(origin, direction, tmin, &tmax))
    {
        return false;
    }

    glm::vec3 current_point = origin;
    float current_sdf = std::numeric_limits<float>::infinity();
    float marched_distance = 0.0f;

    while(true)
    {
        current_sdf = _sdf_func(current_point);
        if(current_sdf < 1e-6f)
        {
            intersect_info._uv = glm::vec2(0.0f);
            intersect_info._coord = current_point;
            intersect_info._mat = _mat;
            intersect_info._wo = -direction;
            intersect_info._primitive = (Primitive *)this;
            intersect_info._t = marched_distance;
            intersect_info._shading_normal = _sdf_grad(current_point);
            intersect_info._geometry_normal = intersect_info._shading_normal;
            if (glm::dot(intersect_info._geometry_normal, direction) > 0.0f)
            {
                intersect_info._geometry_normal = -intersect_info._geometry_normal;
            }
            return true;
        }
        current_point += direction * current_sdf;
        marched_distance += current_sdf;
        if(marched_distance >= tmax)
        {
            return false;
        }
    }
}
