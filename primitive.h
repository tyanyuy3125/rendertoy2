#pragma once

#include <glm/glm.hpp>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <string>

#include "rendertoy_internal.h"
#include "accelerate.h"

namespace rendertoy
{
    class IMaterial;
    class SurfaceLight;

    class Primitive
    {
    protected:
        std::shared_ptr<IMaterial> _mat = nullptr;
    public: // TODO: 临时措施
        SurfaceLight *_surface_light = nullptr;

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
        virtual const void GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const = 0;
        virtual const float GetArea() const = 0;
        virtual const SurfaceLight *GetSurfaceLight() const;
        virtual const float Pdf(const glm::vec3 &observation_to_primitive, const glm::vec2 &uv) const;
        virtual const glm::vec3 GetNormal(const glm::vec2 &uv) const;
        virtual ~Primitive() {}
    };

    class Triangle : public Primitive
    {
    private:
        glm::vec3 _vert[3]; // Relative coordinate to the nearest origin.
        glm::vec2 _uv[3];
        glm::vec3 _norm[3];

    public:
        Triangle(const glm::vec3 &p0, 
                 const glm::vec3 &p1, 
                 const glm::vec3 &p2, 
                 const glm::vec2 &t0 = glm::vec2(0.0f), 
                 const glm::vec2 &t1 = glm::vec2(0.0f), 
                 const glm::vec2 &t2 = glm::vec2(0.0f),
                 const glm::vec3 &n0 = glm::vec3(0.0f),
                 const glm::vec3 &n1 = glm::vec3(0.0f),
                 const glm::vec3 &n2 = glm::vec3(0.0f))
        : _vert{p0, p1, p2}, _uv{t0, t1, t2}, _norm{n0, n1, n2} {}
        Triangle(const aiVector3D &p0, 
                 const aiVector3D &p1, 
                 const aiVector3D &p2, 
                 const aiVector2D &t0, 
                 const aiVector2D &t1, 
                 const aiVector2D &t2,
                 const aiVector3D &n0,
                 const aiVector3D &n1,
                 const aiVector3D &n2)
        : _vert{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), glm::vec3(p2.x, p2.y, p2.z)},
          _uv{glm::vec2(t0.x, t0.y), glm::vec2(t1.x, t1.y), glm::vec2(t2.x, t2.y)},
          _norm{glm::vec3(n0.x, n0.y, n0.z), glm::vec3(n1.x, n1.y, n1.z), glm::vec3(n2.x, n2.y, n2.z)}
        {}
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
        virtual const float GetArea() const;
        virtual const SurfaceLight *GetSurfaceLight() const;
        virtual const void GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const;
        virtual const float Pdf(const glm::vec3 &observation_to_primitive, const glm::vec2 &uv) const;
        virtual const glm::vec3 GetNormal(const glm::vec2 &uv) const;
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
        virtual const float GetArea() const;
        virtual const void GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const;
    };

    class TriangleMesh : public Primitive
    {
    private:
        BVH<Triangle> _triangles;
        BBox _bbox;

    public:
        TriangleMesh() = default;
        TriangleMesh(const TriangleMesh &) = delete;
        const std::vector<std::shared_ptr<Triangle>> &triangles() const{
            return _triangles.objects;
        }
        friend const std::vector<std::shared_ptr<TriangleMesh>> ImportMeshFromFile(const std::string &path);
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
        virtual const float GetArea() const;
        virtual const void GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const;
    };
}