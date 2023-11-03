#pragma once

#include <glm/glm.hpp>

#include "rendertoy_internal.h"
#include "composition.h"

namespace rendertoy
{
    enum class SampleMethod
    {
        NEAREST_NEIGHBOUR = 0,
        BILINEAR,
    };

    template <typename T>
    class ISamplable
    {
    protected:
        SampleMethod _sample_method = SampleMethod::NEAREST_NEIGHBOUR;

    public:
        void SetSampleMethod(const SampleMethod sample_method)
        {
            _sample_method = sample_method;
        }

        virtual const T Sample(const float u, const float v) const = 0;
        const T Sample(const glm::vec2 &uv) const
        {
            return Sample(uv.x, uv.y);
        }

        virtual const T Avg() const = 0;
    };

    typedef ISamplable<glm::vec4> ISamplableColor;
    typedef ISamplable<float> ISamplableNumerical;

    class ConstantNumerical : public ISamplable<float>
    {
    private:
        float _value;

    public:
        ConstantNumerical(const float value) : _value(value) {}

        virtual const float Sample(const float u, const float v) const
        {
            return _value;
        }

        virtual const float Avg() const
        {
            return _value;
        }
    };

    class ImageTexture : public ISamplable<glm::vec4>
    {
    private:
        Image _image;

    public:
        ImageTexture(const Image &image) : _image(image) {}
        ImageTexture(const int width, const int height) : _image(width, height) {}
        ImageTexture(const std::string &path);

        virtual const glm::vec4 Sample(const float u, float v) const
        {
            v = 1.0f - v;
            switch (_sample_method)
            {
            case SampleMethod::NEAREST_NEIGHBOUR:
            {
                return _image(static_cast<int>(u * (_image.width() - 1)), static_cast<int>(v * (_image.height()-1)));
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

                glm::vec4 c00 = _image(x0, y0);
                glm::vec4 c01 = _image(x0, y1);
                glm::vec4 c10 = _image(x1, y0);
                glm::vec4 c11 = _image(x1, y1);

                glm::vec4 color = (1.0f - tx) * (1.0f - ty) * c00 + tx * (1.0f - ty) * c10 + (1.0f - tx) * ty * c01 + tx * ty * c11;
                return glm::vec4(color);
                break;
            }
            }
        }

        virtual const glm::vec4 Avg() const
        {
            return _image.Avg();
        }
    };

    class ColorTexture : public ISamplable<glm::vec4>
    {
    private:
        glm::vec4 _color;

    public:
        ColorTexture(const glm::vec4 &color) : _color(color) {}

        virtual const glm::vec4 Sample(const float u, const float v) const
        {
            return _color;
        }

        virtual const glm::vec4 Avg() const
        {
            return _color;
        }
    };
}
