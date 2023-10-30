#include <memory>

#include "rendertoy_internal.h"
#include "composition.h"
#include "camera.h"
#include "scene.h"

namespace rendertoy
{
    struct RenderConfig
    {
        // Film resolution
        int width = 1920;
        int height = 1080;

        std::shared_ptr<Camera> camera;
        std::shared_ptr<Scene> scene;

        // Depth buffer
        glm::float32 near = 0.1f;
        glm::float32 far = 100.0f;
    };

    class IRenderWork
    {
    protected:
        RenderConfig _render_config;
        Image _output;
    public:
        IRenderWork() = delete;
        IRenderWork(RenderConfig render_config);
        virtual void Render() = 0;
        
        const Image &GetResult() const;
    };

    class TestRenderWork : public IRenderWork
    {
    public:
        virtual void Render();
        TestRenderWork() = delete;
        TestRenderWork(RenderConfig render_config);
    };

    class DepthBufferRenderWork : public IRenderWork
    {
    public:
        virtual void Render();
        DepthBufferRenderWork() = delete;
        DepthBufferRenderWork(RenderConfig render_config);
    };
}