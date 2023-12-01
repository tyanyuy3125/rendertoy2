#pragma once

#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <string>

#include "rendertoy_internal.h"
#include "accelerate.h"

#define PRIMITIVE_METADATA(type)                      \
private:                                              \
    unsigned int _primitive_type = type;              \
                                                      \
public:                                               \
    virtual const unsigned int PRIMITIVE_TYPE() const \
    {                                                 \
        return _primitive_type;                       \
    }

namespace rendertoy
{
    class Primitive
    {
        PRIMITIVE_METADATA(FUNDAMENTAL_PRIMITIVE)
    protected:
        std::shared_ptr<IMaterial> _mat = nullptr;
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
        virtual const glm::vec3 GetCenter() const = 0;
        virtual ~Primitive() {}

        friend class Scene;
    };

    class Triangle : public Primitive
    {
        PRIMITIVE_METADATA(FUNDAMENTAL_PRIMITIVE)
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
        {
        }
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
        virtual const float GetArea() const;
        virtual const SurfaceLight *GetSurfaceLight() const;
        virtual const void GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const;
        /// @brief 在三角形上采样到(u, v)的PDF
        /// @param observation_to_primitive
        /// @param uv
        /// @return
        virtual const float Pdf(const glm::vec3 &observation_to_primitive, const glm::vec2 &uv) const;
        virtual const glm::vec3 GetNormal(const glm::vec2 &uv) const;
        virtual const glm::vec3 GetCenter() const
        {
            return (_vert[0] + _vert[1] + _vert[2]) / 3.0f;
        }
    };

    class TriangleMesh : public Primitive
    {
        PRIMITIVE_METADATA(COMBINED_PRIMITIVE)
    private:
        BVH<Triangle> _triangles;
        BBox _bbox;

    public:
        glm::quat _rot_from = glm::quat(glm::mat3(1.0f));
        glm::vec3 _tran_from = glm::vec3(0.0f);

        glm::quat _rot_to;
        glm::vec3 _tran_to = glm::vec3(0.0f);

        glm::float32 _time_from, _time_to;
        bool _is_animated;

        void GetCurrentAnimationState(const float time, glm::quat &rot, glm::vec3 &tran) const;
        void Animate(const glm::quat &rot_to, const glm::vec3 &tran_to, const glm::float32 time_from, const glm::float32 time_to);

        TriangleMesh() = default;
        TriangleMesh(const TriangleMesh &) = delete;
        const std::vector<std::shared_ptr<Triangle>> &triangles() const
        {
            return _triangles.objects;
        }
        friend const std::vector<std::shared_ptr<TriangleMesh>> ImportMeshFromFile(const std::string &path);
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
        virtual const glm::vec3 GetCenter() const;
        virtual const float GetArea() const;
        virtual const void GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const;
    };

    class SDF : public Primitive
    {
        PRIMITIVE_METADATA(FUNDAMENTAL_PRIMITIVE)
    public:
        using SDFFunction = std::function<float(glm::vec3)>;
        using SDFGrad = std::function<glm::vec3(glm::vec3)>;

    private:
        SDFFunction _sdf_func;
        SDFGrad _sdf_grad;
        BBox _bbox;

    public:
        SDF() = delete;
        SDF(const SDF &) = delete;
        SDF(SDFFunction sdf_func, SDFGrad sdf_grad, BBox bbox)
        : _sdf_func(sdf_func), _sdf_grad(sdf_grad), _bbox(bbox) {}
        virtual const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo &intersect_info) const final;
        virtual const BBox GetBoundingBox() const;
        virtual const glm::vec3 GetCenter() const;
        virtual const float GetArea() const;
        virtual const void GenerateSamplePointOnSurface(glm::vec2 &uv, glm::vec3 &coord, glm::vec3 &normal) const;
    };
}