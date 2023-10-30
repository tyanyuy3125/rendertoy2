#include <glm/gtc/type_ptr.hpp>

#include "primitive.h"

const bool rendertoy::TriangleMesh::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
{
    return triangles.Intersect(origin, direction, intersect_info);
}

const rendertoy::BBox rendertoy::TriangleMesh::GetBoundingBox() const
{
    return _bbox;
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

rendertoy::Triangle::Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec2 &t0, const glm::vec2 &t1, const glm::vec2 &t2)
    : _vert{p0, p1, p2}, _uv{t0, t1, t2}
{
}

rendertoy::Triangle::Triangle(const aiVector3D &p0, const aiVector3D &p1, const aiVector3D &p2, const aiVector2D &t0, const aiVector2D &t1, const aiVector2D &t2)
    : _vert{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), glm::vec3(p2.x, p2.y, p2.z)}, _uv{glm::vec2(t0.x, t0.y), glm::vec2(t1.x, t1.y), glm::vec2(t2.x, t2.y)}
{
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
    intersect_info._t = t;
    // intersect_info.material = triangle_materials[i];
    intersect_info._normal = glm::normalize(glm::cross(v0v1, v0v2));
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
