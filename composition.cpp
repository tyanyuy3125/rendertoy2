#include "composition.h"

#include <memory>
#include <OpenImageIO/imageio.h>
#include <OpenImageDenoise/oidn.hpp>

const glm::vec3 &rendertoy::Image::operator()(const int x, const int y) const
{
    return _buffer[y * _width + x];
}

glm::vec3 &rendertoy::Image::operator()(const int x, const int y)
{
    return _buffer[y * _width + x];
}

const int rendertoy::Image::width() const
{
    return _width;
}

const int rendertoy::Image::height() const
{
    return _height;
}

rendertoy::Image::Image(const int width, const int height) : _width(width), _height(height)
{
    _buffer.resize(width * height, glm::vec3{0.0f});
}

void rendertoy::Image::Export(const std::string &filename) const
{
    std::unique_ptr<OIIO::ImageOutput> out = OIIO::ImageOutput::create(filename);
    OIIO::ImageSpec spec(_width, _height, 3, OIIO::TypeDesc::FLOAT);
    out->open(filename, spec);
    glm::float32 *buffer_as_float = (float*)(_buffer.data());
    out->write_image(OIIO::TypeDesc::FLOAT, buffer_as_float);
    out->close();
}

void rendertoy::Image::PixelShade(const PixelShader &shader)
{
    for(int x = 0; x < _width; ++x)
    {
        for(int y = 0; y < _height; ++y)
        {
            (*this)(x, y) = shader(x, y);
        }
    }
}
