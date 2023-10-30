#pragma once

#include <glm/glm.hpp>

#include "composition.h"
#include "importer.h"

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
        ImageTexture(const int width, const int height) : _image(width, height) {}
        ImageTexture(const std::string &path) : _image(ImportImageFromFile(path)) {}

        virtual const glm::vec4 Sample(const float u, const float v)
        {
            switch (_SM)
            {
            case SampleMethod::NEAREST_NEIGHBOUR:
            {
                return _image(static_cast<int>(u * _image.width()), static_cast<int>(v * _image.height()));
                break;
            }
            case SampleMethod::BILINEAR:
            {
                int x0 = static_cast<int>(u * (_image.width() - 1));
                int y0 = static_cast<int>(v * (_image.height() - 1));
                int x1 = x0 + 1;
                int y1 = y0 + 1;

                float tx = u * (_image.width() - 1.0f) - x0;
                float ty = v * (_image.height() - 1.0f) - y0;

                glm::vec3 c00 = _image(x0, y0);
                glm::vec3 c01 = _image(x0, y1);
                glm::vec3 c10 = _image(x1, y0);
                glm::vec3 c11 = _image(x1, y1);

                glm::vec3 color = (1.0f - tx) * (1.0f - ty) * c00 + tx * (1.0f - ty) * c10 + (1.0f - tx) * ty * c01 + tx * ty * c11;
                return color;
                break;
            }
            }
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
