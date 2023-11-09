#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <atomic>

#include "rendertoy_internal.h"
#include "color.h"

namespace rendertoy
{
    typedef std::function<glm::vec4(const int, const int)> PixelShader;
    typedef std::function<glm::vec4(const glm::vec2 &)> PixelShaderSSAA;
    typedef std::function<glm::vec3(const glm::vec2 &)> RayTracingShader;

    class Image
    {
    private:
        glm::vec4 _;
        std::vector<glm::vec4> _buffer;
        int _width;
        int _height;

    public:
        const std::vector<glm::vec4> &GetBuffer() const
        {
            return _buffer;
        }

        const glm::vec4 &operator()(const int x, const int y) const;
        glm::vec4 &operator()(const int x, const int y);

        const int width() const;
        const int height() const;

        Image() = delete;
        Image(const int width, const int height, const glm::vec4 &fill = glm::vec4(0.0f));

        void Export(const std::string &filename, const ColorSpace color_space = ColorSpace::SRGB) const;

        void PixelShade(const PixelShader &shader);
        void PixelShadeSSAA(const PixelShaderSSAA &shader, const int x_sample, const int y_sample);
        void RayTrace(const RayTracingShader &shader, const int x_sample, const int y_sample, const int spp, const float max_noise_tolerance);

        const Image UpScale(const glm::float32 factor) const;
        const Image NextMipMap() const;

        friend const Image ImportImageFromFile(const std::string &path);

        const glm::vec4 Avg() const;
    };

    enum class MixMode
    {
        NORMAL = 0,
        DIFF,
        MAX,
        NORMAL_CLAMP,
        DIFF_CLAMP,
        INVERT
    };

    struct Layer
    {
        std::shared_ptr<Image> _image;
        glm::ivec2 _position;
        MixMode _mix_mode;

        Layer(std::shared_ptr<Image> image, glm::ivec2 position, MixMode mix_mode = MixMode::NORMAL);
    };

    class Canvas
    {
    private:
        std::vector<Layer> _layers;
        int _width;
        int _height;
        
    public:
        Canvas(int width, int height);
        const Image ToImage() const;

        const std::vector<Layer> &layers() const;
        std::vector<Layer> &layers();
    };
}