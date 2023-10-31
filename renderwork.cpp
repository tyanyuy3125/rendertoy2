#include <memory>

#include "renderwork.h"
#include "dotfont.h"
#include "composition.h"
#include "material.h"

#include <glm/glm.hpp>
#include <chrono>
#include <glm/gtc/constants.hpp>
#include <cmath>

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
    int triangle_count = 0;
    for (const auto &object : _render_config.scene->objects())
    {
        auto object_raw_ptr = object.get();
        if (object_raw_ptr && typeid(*object_raw_ptr) == typeid(TriangleMesh))
        {
            TriangleMesh *tmptr = dynamic_cast<TriangleMesh *>(object.get());
            triangle_count += tmptr->triangles().size();
        }
    }
    Image text = GenerateTextImage({std::string("RenderToy2 Build ") + std::to_string(BUILD_NUMBER) + std::string("+") + std::string(BUILD_DATE),
                                    std::string("Film: ") + std::to_string(_render_config.width) + std::string("x") + std::to_string(_render_config.height),
                                    std::string("RenderWork Type: ") + this->GetClassName(),
                                    std::string("Time elapsed: ") + std::to_string(_stat.time_elapsed) + std::string("s"),
                                    std::string("SSAA: " + std::to_string(_render_config.x_sample) + "x" + std::to_string(_render_config.y_sample)),
                                    std::string("Triangles: ") + std::to_string(triangle_count)},
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
    PixelShaderSSAA shader = [&](const glm::vec2 &screen_coord) -> glm::vec4
    {
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
    _output.PixelShadeSSAA(shader, _render_config.x_sample, _render_config.y_sample);
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
    PixelShaderSSAA shader = [&](const glm::vec2 &screen_coord) -> glm::vec4
    {
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
    _output.PixelShadeSSAA(shader, _render_config.x_sample, _render_config.y_sample);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    _stat.time_elapsed = elapsed_time.count();
}

rendertoy::NormalRenderWork::NormalRenderWork(RenderConfig render_config)
    : IRenderWork(render_config)
{
}

static glm::vec2 GetUVOnSkySphere(const glm::vec3 &normal)
{
    // 将法向量转换为球坐标系
    float theta = std::acos(normal.y);          // theta表示纬度
    float phi = std::atan2(normal.z, normal.x); // phi表示经度

    // 将球坐标系映射到UV坐标
    float u = phi / (2.0f * glm::pi<float>()) + 0.5f;
    float v = 1.0f - (theta / glm::pi<float>());

    return glm::vec2(u, v);
}

void rendertoy::AlbedoRenderWork::Render()
{
    int width = _output.width();
    int height = _output.height();
    PixelShaderSSAA shader = [&](const glm::vec2 &screen_coord) -> glm::vec4
    {
        glm::vec3 origin, direction;
        IntersectInfo intersect_info;
        _render_config.camera->SpawnRay(screen_coord, origin, direction);
        if (_render_config.scene->Intersect(origin, direction, intersect_info))
        {
            if (intersect_info._mat != nullptr)
            {
                return intersect_info._mat->albedo()->Sample(intersect_info._uv);
            }
        }
        return _render_config.scene->hdr_background()->Sample(GetUVOnSkySphere(direction));
    };
    auto start_time = std::chrono::high_resolution_clock::now();
    _output.PixelShadeSSAA(shader, _render_config.x_sample, _render_config.y_sample);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    _stat.time_elapsed = elapsed_time.count();
}

rendertoy::AlbedoRenderWork::AlbedoRenderWork(RenderConfig render_config)
    : IRenderWork(render_config)
{
}

void rendertoy::PathTracingRenderWork::Render()
{
    // 0: [1 * emissive0] + reflectance0 * RADIANCE
    // 1: emissive0 + reflectance0 * (emissive1 + reflectance1 * RADIANCE)
    //   =[1 * emissive0 + reflectance0 * emissive1] + reflectance0 * reflectance1 * RADIANCE
    // 2: emissive0 + reflectance0 * (emissive1 + reflectance1 * (emissive2 + reflectance2 * RADIANCE))
    //   =[1 * emissive0 + reflectance0 * emissive1 + reflectance0 * reflectance1 * emissive2] + reflectance0 * reflectance1 * reflectance2 * RADIANCE
    int width = _output.width();
    int height = _output.height();
    PixelShaderSSAA shader = [&](const glm::vec2 &screen_coord) -> glm::vec4
    {
        glm::vec3 ret = glm::vec3(0.0f);
        for(int i=0;i<4;++i)
        {
            glm::vec3 factor = glm::vec3(1.0f);
            glm::vec3 ret_per_iter = glm::vec3(0.0f);
            glm::vec3 origin, direction;
            IntersectInfo intersect_info;
            _render_config.camera->SpawnRay(screen_coord, origin, direction);
            for(int j=0;j<16;++j)
            {
                if (_render_config.scene->Intersect(origin, direction, intersect_info))
                {
                    ret_per_iter += factor * intersect_info._mat->EvalEmissive(intersect_info);
                    factor *= intersect_info._mat->Eval(intersect_info);
                    origin = intersect_info._coord;
                    direction = intersect_info.GenerateSurfaceCoordinates() * intersect_info._mat->Sample(intersect_info);
                }
                else
                {
                    ret_per_iter += factor * glm::vec3(_render_config.scene->hdr_background()->Sample(GetUVOnSkySphere(direction)));
                    break;
                }
            }
            ret += ret_per_iter / 4.0f;
        }
        return glm::vec4(ret, 1.0f);
    };
    auto start_time = std::chrono::high_resolution_clock::now();
    _output.PixelShadeSSAA(shader, _render_config.x_sample, _render_config.y_sample);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    _stat.time_elapsed = elapsed_time.count();
}
