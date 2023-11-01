#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "primitive.h"
#include "logger.h"

const bool rendertoy::TriangleMesh::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
{
    bool ret = _triangles.Intersect(origin, direction, intersect_info);
    if(intersect_info._mat == nullptr)
    {
        intersect_info._mat = _mat;
    }
    return ret;
}

const rendertoy::BBox rendertoy::TriangleMesh::GetBoundingBox() const
{
    return _bbox;
}

const float rendertoy::TriangleMesh::GetArea() const
{
    CRIT << "Not implemented." <<std::endl;
    return 0.0f;
}

const void rendertoy::TriangleMesh::GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const
{
    int idx = glm::linearRand<int>(0, static_cast<int>(this->triangles().size()) - 1);
    this->triangles()[idx]->GenerateSamplePointOnSurface(uv, coord, normal);
}

rendertoy::UVSphere::UVSphere(const glm::vec3 &origin, const float &radius)
    : _origin(origin), _radius(radius)
{
}

const bool rendertoy::UVSphere::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
{
    glm::vec3 a = _origin - origin;
    float l = glm::dot(direction, a);
    float a2 = glm::dot(a, a);
    float t = 0;
    if (a2 > _radius * _radius && l < 0)
    {
        return false;
    }
    float m2 = a2 - l * l;
    float R2 = _radius * _radius;
    if (m2 > R2)
    {
        return false;
    }
    float q = sqrt(R2 - m2);
    if (a2 > R2)
    {
        t = l - q;
    }
    else
    {
        t = l + q;
    }
    if (t < 1e-3)
    {
        return false;
    }
    intersect_info._t = t;
    intersect_info._normal = glm::normalize(origin + t * direction - _origin);
    if (glm::dot(intersect_info._normal, direction) > 0)
    {
        intersect_info._normal = -intersect_info._normal;
    }
    return true;
}

const rendertoy::BBox rendertoy::UVSphere::GetBoundingBox() const
{
    return BBox(_origin - glm::vec3(_radius), _origin + glm::vec3(_radius));
}

const float rendertoy::UVSphere::GetArea() const
{
    CRIT << "Not implemented." <<std::endl;
    return 0.0f;
}

const void rendertoy::UVSphere::GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const
{
    return void();
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
    // intersect_info._normal = glm::normalize(glm::cross(v0v1, v0v2));
    intersect_info._normal = u * _norm[1] + v * _norm[2] + (1 - u - v) * _norm[0];
    intersect_info._mat = _mat;
    intersect_info._in = -direction;
    if (glm::dot(intersect_info._normal, direction) > 0.0f)
    {
        intersect_info._normal = -intersect_info._normal;
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

const void rendertoy::Triangle::GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const
{
    float u = glm::linearRand<float>(0.0f, 1.0f);
    float v = glm::linearRand<float>(0.0f, 1.0f);
    glm::vec3 v0v1 = _vert[1] - _vert[0];
    glm::vec3 v0v2 = _vert[2] - _vert[0];
    if(u + v > 1.0f)
    {
        u = 1.0f - u;
        v = 1.0f - v;
    }
    coord = u * v0v1 + v * v0v2;
    uv = glm::vec2(u, v);
    normal = u * _norm[1] + v * _norm[2] + (1 - u - v) * _norm[0];
}
