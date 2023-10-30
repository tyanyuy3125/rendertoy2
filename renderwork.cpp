#include "renderwork.h"

#include <glm/glm.hpp>

void rendertoy::TestRenderWork::Render()
{
    int width = _output.width();
    int height = _output.height();
    PixelShader shader = [width, height](const int x, const int y) -> glm::vec3 
    {
        glm::float32 r = static_cast<glm::float32>(x) / static_cast<glm::float32>(width);
        glm::float32 g = static_cast<glm::float32>(y) / static_cast<glm::float32>(height);
        glm::float32 b = 1.0f - r - g;
        return glm::vec3{r, g, b};
    };
    _output.PixelShade(shader);
}

rendertoy::TestRenderWork::TestRenderWork(RenderConfig render_config)
: IRenderWork(render_config)
{
}

rendertoy::IRenderWork::IRenderWork(RenderConfig render_config)
: _output(render_config.width, render_config.height), _render_config(render_config)
{
}

const rendertoy::Image &rendertoy::IRenderWork::GetResult() const
{
    return _output;
}

void rendertoy::DepthBufferRenderWork::Render()
{
    int width = _output.width();
    int height = _output.height();
    PixelShader shader = [&](const int x, const int y) -> glm::vec3 
    {
        glm::vec2 screen_coord(static_cast<float>(x), static_cast<float>(y));
        glm::vec3 origin, direction;
        IntersectInfo intersect_info;
        _render_config.camera->SpawnRay(screen_coord, origin, direction);
        if(_render_config.scene->Intersect(origin, direction, intersect_info))
        {
            return glm::vec3(glm::mix(_render_config.far, _render_config.near, intersect_info._t));
        }
        return glm::vec3(0.0f);
    };
    _output.PixelShade(shader);
}

rendertoy::DepthBufferRenderWork::DepthBufferRenderWork(RenderConfig render_config)
    : IRenderWork(render_config)
{
}
