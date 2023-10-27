#include "renderer.h"

#include <glm/glm.hpp>

void rendertoy::TestRenderer::Render()
{
    int width = _output.width();
    int height = _output.height();
    PixelShader shader = [width, height](const int x, const int y) -> glm::vec3 
    {
        glm::float32 r = static_cast<glm::float32>(x) / static_cast<glm::float32>(width);
        glm::float32 g = static_cast<glm::float32>(y) / static_cast<glm::float32>(height);
        glm::float32 b = 1.0f - r - b;
        return glm::vec3{r, g, b};
    };
    _output.PixelShade(shader);
}

rendertoy::TestRenderer::TestRenderer(RenderConfig render_config)
: IRenderer(render_config)
{
}

rendertoy::IRenderer::IRenderer(RenderConfig render_config)
: _output(render_config.width, render_config.height)
{
}

const rendertoy::Image &rendertoy::IRenderer::GetResult() const
{
    return _output;
}
