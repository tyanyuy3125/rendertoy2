#include <memory>

#include "renderwork.h"
#include "dotfont.h"
#include "composition.h"

#include <glm/glm.hpp>
#include <chrono>

void rendertoy::TestRenderWork::Render()
{
    int width = _output.width();
    int height = _output.height();
    PixelShader shader = [width, height](const int x, const int y) -> glm::vec4
    {
        glm::float32 r = static_cast<glm::float32>(x) / static_cast<glm::float32>(width);
        glm::float32 g = static_cast<glm::float32>(y) / static_cast<glm::float32>(height);
        glm::float32 b = 1.0f - r - g;
        return glm::vec4{r, g, b, 1.0f};
    };
    auto start_time = std::chrono::high_resolution_clock::now();
    _output.PixelShade(shader);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    _stat.time_elapsed = elapsed_time.count();
}

rendertoy::TestRenderWork::TestRenderWork(RenderConfig render_config)
    : IRenderWork(render_config)
{
}

rendertoy::IRenderWork::IRenderWork(RenderConfig render_config)
    : _output(render_config.width, render_config.height), _render_config(render_config)
{
}

const rendertoy::Image rendertoy::IRenderWork::GetResult(const bool print_verbose) const
{
    if (!print_verbose)
        return _output;
    Image text = GenerateTextImage({std::string("RenderToy2 Build ") + std::to_string(BUILD_NUMBER) + std::string("+") + std::string(BUILD_DATE),
                                    std::string("Film: ") + std::to_string(_render_config.width) + std::string("x") + std::to_string(_render_config.height),
                                    std::string("RenderWork Type: ") + this->GetClassName(),
                                    std::string("Time elapsed: ") + std::to_string(_stat.time_elapsed) + std::string("s")},
                                   glm::vec4(1.0f), 2);
    Canvas canvas(_render_config.width, _render_config.height);
    canvas.layers().push_back(Layer(std::make_shared<Image>(_output), {0, 0}));
    canvas.layers().push_back(Layer(std::make_shared<Image>(text), {0, 0}, MixMode::INVERT));
    Image ret = canvas.ToImage();
    return ret;
}

void rendertoy::DepthBufferRenderWork::Render()
{
    int width = _output.width();
    int height = _output.height();
    PixelShader shader = [&](const int x, const int y) -> glm::vec4
    {
        glm::vec2 screen_coord(static_cast<float>(x) / static_cast<float>(width), static_cast<float>(y) / static_cast<float>(height));
        glm::vec3 origin, direction;
        IntersectInfo intersect_info;
        _render_config.camera->SpawnRay(screen_coord, origin, direction);
        if (_render_config.scene->Intersect(origin, direction, intersect_info))
        {
            return glm::vec4(glm::vec3((intersect_info._t - _render_config.near) / (_render_config.far - _render_config.near)), 1.0f);
        }
        return glm::vec4(1.0f);
    };
    auto start_time = std::chrono::high_resolution_clock::now();
    _output.PixelShade(shader);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    _stat.time_elapsed = elapsed_time.count();
}

rendertoy::DepthBufferRenderWork::DepthBufferRenderWork(RenderConfig render_config)
    : IRenderWork(render_config)
{
}

void rendertoy::NormalRenderWork::Render()
{
    int width = _output.width();
    int height = _output.height();
    PixelShader shader = [&](const int x, const int y) -> glm::vec4
    {
        glm::vec2 screen_coord(static_cast<float>(x) / static_cast<float>(width), static_cast<float>(y) / static_cast<float>(height));
        glm::vec3 origin, direction;
        IntersectInfo intersect_info;
        _render_config.camera->SpawnRay(screen_coord, origin, direction);
        if (_render_config.scene->Intersect(origin, direction, intersect_info))
        {
            return glm::vec4(intersect_info._normal, 1.0f);
        }
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    };
    auto start_time = std::chrono::high_resolution_clock::now();
    _output.PixelShade(shader);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    _stat.time_elapsed = elapsed_time.count();
}

rendertoy::NormalRenderWork::NormalRenderWork(RenderConfig render_config)
    : IRenderWork(render_config)
{
}
