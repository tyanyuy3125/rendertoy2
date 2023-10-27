#include "rendertoy_internal.h"

#include "composition.h"

namespace rendertoy
{
    struct RenderConfig
    {
        int width;
        int height;
    };

    class IRenderer
    {
    protected:
        Image _output;
    public:
        IRenderer() = delete;
        IRenderer(RenderConfig render_config);
        virtual void Render() = 0;
        
        const Image &GetResult() const;
    };

    class TestRenderer : public IRenderer
    {
    public:
        virtual void Render();
        TestRenderer() = delete;
        TestRenderer(RenderConfig render_config);
    };
}