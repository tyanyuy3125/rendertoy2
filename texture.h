#pragma once

#include <glm/glm.hpp>

#include "composition.h"

namespace rendertoy
{
    template <SampleMethod _SM = SampleMethod::NEAREST_NEIGHBOUR>
    class ISamplable
    {
    public:
        virtual const glm::vec4 Sample(const float u, const float v) = 0;
    };

    enum class SampleMethod
    {
        NEAREST_NEIGHBOUR = 0,
        BILINEAR,
    };

    template <SampleMethod _SM = SampleMethod::NEAREST_NEIGHBOUR>
    class ImageTexture : public ISamplable<_SM>
    {
    private:
        Image _image;

    public:
        ImageTexture(const Image &image) : _image(image) {}
        ImageTexture(const int width, const int height);
        ImageTexture(const std::string &path);

        virtual const glm::vec4 Sample(const float u, const float v)
        {
            
        }
    };

    template <SampleMethod _SM = SampleMethod::NEAREST_NEIGHBOUR>
    class ColorTexture : public ISamplable<_SM>
    {
    private:
        glm::vec4 _color;

    public:
        ColorTexture(const glm::vec4 &color) : _color(color) {}

        virtual const glm::vec4 Sample(const float u, const float v)
        {
            return _color;
        }
    };
}
