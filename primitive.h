#pragma once

#include <glm/glm.hpp>

#include "rendertoy_internal.h"
#include "accelerate.h"
#include "intersectinfo.h"

namespace rendertoy
{
    class Primitive
    {
    private:
        glm::vec3 origin;
    public:
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const = 0;
        virtual const BBox GetBoundingBox() const = 0;
    };

    class Triangle : public Primitive
    {
    public:
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
    };

    class UVSphere : public Primitive
    {
    private:
        glm::float32 radius;
    public:
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
    };

    class TriangleMesh : public Primitive
    {
    public:
        BVH<Triangle> triangles;
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info);
        virtual const BBox GetBoundingBox() const;
    };
}