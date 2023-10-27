#include "primitive.h"

const bool rendertoy::TriangleMesh::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info)
{
    return false;
}

const rendertoy::BBox rendertoy::TriangleMesh::GetBoundingBox() const
{
    throw;
}

const bool rendertoy::UVSphere::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
{
    return false;
}

const rendertoy::BBox rendertoy::UVSphere::GetBoundingBox() const
{
    throw;
}

const rendertoy::BBox rendertoy::Triangle::GetBoundingBox() const
{
    throw;
}
