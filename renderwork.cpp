#include <memory>

#include "rendertoy_internal.h"
#include "renderwork.h"
#include "dotfont.h"
#include "composition.h"
#include "material.h"
#include "sampler.h"
#include "texture.h"
#include "camera.h"
#include "primitive.h"
#include "scene.h"
#include "light.h"
#include "bxdf.h"

#include <OpenImageDenoise/oidn.hpp>
#include <chrono>
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
            triangle_count += static_cast<int>(tmptr->triangles().size());
        }
    }
    Image text = GenerateTextImage({std::string("RenderToy2 Build ") + std::to_string(BUILD_NUMBER) + std::string("+") + std::string(BUILD_DATE),
                                    std::string("film: ") + std::to_string(_render_config.width) + std::string("x") + std::to_string(_render_config.height),
                                    std::string("renderwork Type: ") + this->GetClassName(),
                                    std::string("time elapsed: ") + std::to_string(_stat.time_elapsed) + std::string("s"),
                                    std::string("ssaa: " + std::to_string(_render_config.x_sample) + "x" + std::to_string(_render_config.y_sample)),
                                    std::string("triangles: ") + std::to_string(triangle_count),
                                    std::string("spp: ") + std::to_string(_render_config.spp),
                                    std::string("exposure: ") + std::to_string(_render_config.exposure) + std::string("s"),
                                    std::string("gamma: " + std::to_string(_render_config.gamma))},
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
            return glm::vec4(intersect_info._geometry_normal, 1.0f);
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
    int width = _output.width();
    int height = _output.height();
    RayTracingShader shader = [&](const glm::vec2 &screen_coord) -> glm::vec3
    {
        glm::vec3 factor = glm::vec3(1.0f);
        glm::vec3 L = glm::vec3(0.0f);
        glm::vec3 origin, direction;
        IntersectInfo intersect_info;
        BxDFType sampled_flag;
        glm::vec3 spectrum;
        float pdf_next, pdf_light, pdf_scattering;
        bool specular_bounce = false;
        float eta = 1.0f;
        _render_config.camera->SpawnRay(screen_coord, origin, direction);
        for (int depth = 0; depth < 8; ++depth)
        {
            if (_render_config.scene->Intersect(origin, direction, intersect_info))
            {
                // 如果当前表面是发光表面，则进行直接光源采样的BSDF采样或者亮度项计算
                auto surface_light = intersect_info._primitive->GetSurfaceLight();
                if (surface_light)
                {
                    glm::vec3 Le = surface_light->Sample_Le(origin, intersect_info, pdf_light);
                    if (depth == 0 || specular_bounce)
                    {
                        L += factor * Le;
                    }
                    else
                    {
                        L += factor * PowerHeuristic(1, pdf_next, 1, pdf_light) * Le;
                    }
                }

                // 对当前材质的 BSDF 进行采样
                std::unique_ptr<BSDF> bsdf = intersect_info._mat->GetBSDF(intersect_info);

                // 更新采样光线
                origin = intersect_info._coord;
                spectrum = bsdf->Sample_f(intersect_info._wo, &direction, &pdf_next, BSDF_ALL, &sampled_flag);

                // 更新直接光源采样项
                // 在直接光源采样中，对光源进行采样
                // 如果当前光线打到的表面是 SPECULAR 材质，那么以下步骤是不必要的。因为mat_bsdf = 0.
                if (bsdf->NumComponents(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0)
                {
                    bool consider_normal = !bsdf->IsTransmissive();
                    // bool consider_normal = true;
                    glm::vec3 dls_direction;
                    SurfaceLight *sampled_light = nullptr;
                    glm::vec3 dls_Li = _render_config.scene->SampleLights(intersect_info, pdf_light, dls_direction, sampled_light, consider_normal);
                    glm::vec3 dls_mat_spectrum;
                    if (glm::dot(dls_Li, dls_Li) > 1e-5f)
                    {
                        dls_mat_spectrum = bsdf->f(intersect_info._wo, dls_direction);
                        L += factor * PowerHeuristic(1, pdf_light, 1, pdf_scattering) * std::abs(glm::dot(dls_direction, intersect_info._geometry_normal)) * dls_mat_spectrum * dls_Li / pdf_light;
                    }
                }

                // 在引入微表面采样以后，由于不再采用均匀半球或者余弦采样方法，所以可能会导致采到概率密度为0的部分，需要将其剔除。
                // 如果不进行下述剔除，可能由于数值计算误差产生Inf点或者NaN点。
                if (spectrum == glm::zero<glm::vec3>() || pdf_next == 0.0f)
                {
                    break;
                }

                // 更新当前表面是否为镜面
                specular_bounce = BSDF::IsSpecular(sampled_flag);
                // 更新因子项。在引入透射以后由于法线反转，所以需要引入abs。
                factor = (spectrum / pdf_next) * std::abs(glm::dot(direction, intersect_info._geometry_normal)) * factor;
            }
            else
            {
                // 光线撞击到 HDRI 背景图像 / 纯色背景等可采样管线
                L += factor * glm::vec3(_render_config.scene->hdr_background()->Sample(GetUVOnSkySphere(direction)));
                break;
            }

            // 俄罗斯轮盘赌剪枝
            {
                float p = glm::compMax(factor);
                if (glm::linearRand<float>(0.0f, 1.0f) > p)
                    break;
                factor *= 1.0f / p;
            }
        }
        return L * _render_config.exposure;
    };
    auto start_time = std::chrono::high_resolution_clock::now();
    _output.RayTrace(shader, _render_config.x_sample, _render_config.y_sample, _render_config.spp, _render_config.max_noise_tolerance);
    auto end_time = std::chrono::high_resolution_clock::now();
    PixelShader tone_mapping = [&](const int x, const int y) -> glm::vec4
    {
        // 使用 ACES 色调映射方法执行 HDR 到 LDR 的转换
        float a = 2.51f;
        float b = 0.03f;
        float c = 2.43f;
        float d = 0.59f;
        float e = 0.14f;
        auto color = _output(x, y);
        auto ret = glm::clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f, 1.0f);

        // 使用 sRGB 色调映射方法执行 linear 到 sRGB 的转换
        ret.r = ret.r <= 0.0031308f ? 12.92f * ret.r : 1.055f * glm::pow(ret.r, 1.0f / _render_config.gamma) - 0.055f;
        ret.g = ret.g <= 0.0031308f ? 12.92f * ret.g : 1.055f * glm::pow(ret.g, 1.0f / _render_config.gamma) - 0.055f;
        ret.b = ret.b <= 0.0031308f ? 12.92f * ret.b : 1.055f * glm::pow(ret.b, 1.0f / _render_config.gamma) - 0.055f;
        ret.a = 1.0f;
        return ret;
    };
    _output.PixelShade(tone_mapping);
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    _stat.time_elapsed = elapsed_time.count();
}

void rendertoy::ProductionalRenderWork::Render()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    INFO << "Rendering beauty pass..." << std::endl;
    PathTracingRenderWork ptrw(_render_config);
    ptrw.Render();
    _output = ptrw.GetResult(false);
    INFO << "Rendering albedo pass..." << std::endl;
    AlbedoRenderWork arw(_render_config);
    arw.Render();
    INFO << "Rendering normal pass..." << std::endl;
    NormalRenderWork nrw(_render_config);
    nrw.Render();
    INFO << "Denoising..." << std::endl;
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();
    oidn::FilterRef filter = device.newFilter("RT");
    filter.set("hdr", true);
    filter.setImage("color", (float *)&_output(0, 0), oidn::Format::Float3, _render_config.width, _render_config.height, 0, 16, 0);
    filter.setImage("albedo", (float *)&arw.GetResult(false)(0, 0), oidn::Format::Float3, _render_config.width, _render_config.height, 0, 16, 0);
    filter.setImage("normal", (float *)&nrw.GetResult(false)(0, 0), oidn::Format::Float3, _render_config.width, _render_config.height, 0, 16, 0);
    filter.setImage("output", (float *)&_output(0, 0), oidn::Format::Float3, _render_config.width, _render_config.height, 0, 16, 0);
    filter.commit();
    filter.execute();
    const char *errorMessage;
    if (device.getError(errorMessage) != oidn::Error::None)
    {
        CRIT << "OIDN Error: " << errorMessage << std::endl;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    _stat.time_elapsed = elapsed_time.count();
}
