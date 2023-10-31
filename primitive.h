#pragma once

#include <glm/glm.hpp>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <string>

#include "rendertoy_internal.h"
#include "accelerate.h"
#include "intersectinfo.h"

namespace rendertoy
{
    class IMaterial;

    class Primitive
    {
    protected:
        std::shared_ptr<IMaterial> _mat = nullptr;

    public:
        const std::shared_ptr<IMaterial> &mat() const
        {
            return _mat;
        }
        std::shared_ptr<IMaterial> &mat()
        {
            return _mat;
        }
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const = 0;
        virtual const BBox GetBoundingBox() const = 0;

        virtual ~Primitive() {}
    };

    class Triangle : public Primitive
    {
    private:
        glm::vec3 _vert[3]; // Relative coordinate to the nearest origin.
        glm::vec2 _uv[3];

    public:
        Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec2 &t0 = glm::vec2(0.0f), const glm::vec2 &t1 = glm::vec2(0.0f), const glm::vec2 &t2 = glm::vec2(0.0f));
        Triangle(const aiVector3D &p0, const aiVector3D &p1, const aiVector3D &p2, const aiVector2D &t0, const aiVector2D &t1, const aiVector2D &t2);
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
    };

    class UVSphere : public Primitive
    {
    private:
        glm::vec3 _origin;
        glm::float32 _radius;

    public:
        UVSphere(const glm::vec3 &origin, const float &radius);
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
    };

    class TriangleMesh : public Primitive
    {
    private:
        BVH<Triangle> triangles;
        BBox _bbox;

    public:
        TriangleMesh() = default;
        TriangleMesh(const TriangleMesh &) = delete;
        friend const std::vector<std::unique_ptr<TriangleMesh>> ImportMeshFromFile(const std::string &path);
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
    };
}