#include "composition.h"

#include <memory>
#include <OpenImageIO/imageio.h>
#include <algorithm>

const glm::vec4 &rendertoy::Image::operator()(const int x, const int y) const
{
    if (x < 0 || x >= _width || y < 0 || y >= _height)
    {
        return _;
    }
    return _buffer[y * _width + x];
}

glm::vec4 &rendertoy::Image::operator()(const int x, const int y)
{
    if (x < 0 || x >= _width || y < 0 || y >= _height)
    {
        return _;
    }
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

rendertoy::Image::Image(const int width, const int height, const glm::vec4 &fill) : _width(width), _height(height)
{
    _buffer.resize(width * height, fill);
}

void rendertoy::Image::Export(const std::string &filename) const
{
    std::unique_ptr<OIIO::ImageOutput> out = OIIO::ImageOutput::create(filename);
    OIIO::ImageSpec spec(_width, _height, 4, OIIO::TypeDesc::FLOAT);
    out->open(filename, spec);
    glm::float32 *buffer_as_float = (float *)(_buffer.data());
    out->write_image(OIIO::TypeDesc::FLOAT, buffer_as_float);
    out->close();
}

void rendertoy::Image::PixelShade(const PixelShader &shader)
{
    for (int x = 0; x < _width; ++x)
    {
        for (int y = 0; y < _height; ++y)
        {
            (*this)(x, y) = shader(x, y);
        }
    }
}

rendertoy::Canvas::Canvas(int width, int height)
    : _width(width), _height(height)
{
}

const rendertoy::Image rendertoy::Canvas::ToImage() const
{
    Image ret(_width, _height, glm::vec4(0.0f));
    for (const auto &layer : _layers)
    {
        for (int x = std::max(layer._position.x, 0); x < std::min(_width, layer._position.x + layer._image->width()); ++x)
        {
            for (int y = std::max(layer._position.y, 0); y < std::min(_height, layer._position.y + layer._image->height()); ++y)
            {
                switch (layer._mix_mode)
                {
                default: case MixMode::NORMAL:
                    ret(x, y) += layer._image->operator()(x - layer._position.x, y - layer._position.y);
                    break;
                case MixMode::DIFFERENCE:
                    ret(x, y) = glm::abs(ret(x, y) - layer._image->operator()(x - layer._position.x, y - layer._position.y));
                    break;
                }
            }
        }
    }
    return ret;
}

const std::vector<rendertoy::Layer> &rendertoy::Canvas::layers() const
{
    return _layers;
}

std::vector<rendertoy::Layer> &rendertoy::Canvas::layers()
{
    return _layers;
}

rendertoy::Layer::Layer(std::shared_ptr<Image> image, glm::ivec2 position, MixMode mix_mode)
    : _image(image), _position(position), _mix_mode(mix_mode)
{
}
