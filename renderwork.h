#pragma once

#include <memory>

#include "rendertoy_internal.h"
#include "composition.h"

namespace rendertoy
{
    struct RenderConfig
    {
        // Film
        int width = 1920;
        int height = 1080;
        float exposure = 0.5f;
        float gamma = 2.2f;
        float time = 0.0f;

        std::shared_ptr<Camera> camera;
        std::shared_ptr<Scene> scene;

        // Depth buffer
        glm::float32 _near = 0.1f;
        glm::float32 _far = 100.0f;

        // Antialiasing
        uint32_t x_sample = 4;
        uint32_t y_sample = 4;

        // Path tracing
        int spp = 16;
        float max_noise_tolerance = 0.05f;
    };

    struct RenderStat
    {
        double time_elapsed;
    };

    class IRenderWork
    {
        CLASS_METADATA_MARK(IRenderWork)
    protected:
        RenderConfig _render_config;
        Image _output;
        RenderStat _stat;

    public:
        IRenderWork() = delete;
        IRenderWork(RenderConfig render_config);
        virtual void Render() = 0;

        const Image GetResult(const bool print_verbose = false) const;
    };

    class TestRenderWork : public IRenderWork
    {
        CLASS_METADATA_MARK(TestRenderWork)
    public:
        virtual void Render();
        TestRenderWork() = delete;
        TestRenderWork(RenderConfig render_config);
    };

    class DepthBufferRenderWork : public IRenderWork
    {
        CLASS_METADATA_MARK(DepthBufferRenderWork)
    public:
        virtual void Render();
        DepthBufferRenderWork() = delete;
        DepthBufferRenderWork(RenderConfig render_config);
    };

    class NormalRenderWork : public IRenderWork
    {
        CLASS_METADATA_MARK(NormalRenderWork)
    public:
        virtual void Render();
        NormalRenderWork() = delete;
        NormalRenderWork(RenderConfig render_config);
    };

    class AlbedoRenderWork : public IRenderWork
    {
        CLASS_METADATA_MARK(AlbedoRenderWork)
    public:
        virtual void Render();
        AlbedoRenderWork() = delete;
        AlbedoRenderWork(RenderConfig render_config);
    };

    class PathTracingRenderWork : public IRenderWork
    {
        CLASS_METADATA_MARK(PathTracingRenderWork)
    public:
        virtual void Render();
        PathTracingRenderWork() = delete;
        PathTracingRenderWork(const RenderConfig &render_config)
        : IRenderWork(render_config) {}
    };
#ifndef OIDN_NOT_FOUND
    class ProductionalRenderWork : public IRenderWork
    {
        CLASS_METADATA_MARK(ProductionalRenderWork)
    public:
        virtual void Render();
        ProductionalRenderWork() = delete;
        ProductionalRenderWork(const RenderConfig &render_config)
        : IRenderWork(render_config) {}
    };
#endif // OIDN_NOT_FOUND

    class AORenderWork : public IRenderWork
    {
        CLASS_METADATA_MARK(AORenderWork)
    public:
        virtual void Render();
        AORenderWork() = delete;
        AORenderWork(const RenderConfig &render_config)
        : IRenderWork(render_config) {}
    };
}