#include "composition.h"

#include <memory>
#include <OpenImageIO/imageio.h>
#include <algorithm>
#include <tbb/tbb.h>

#include "logger.h"

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

void rendertoy::Image::Export(const std::string &filename, const ColorSpace color_space) const
{
    std::shared_ptr<OIIO::ImageOutput> out = OIIO::ImageOutput::create(filename);
    OIIO::ImageSpec spec(_width, _height, 4, OIIO::TypeDesc::FLOAT);
    spec.attribute("oiio:ColorSpace", oiio_color_space_string[static_cast<int>(color_space)]);
    out->open(filename, spec);
    glm::float32 *buffer_as_float = (float *)(_buffer.data());
    out->write_image(OIIO::TypeDesc::FLOAT, buffer_as_float);
    out->close();
}

void rendertoy::Image::PixelShade(const rendertoy::PixelShader &shader)
{
#ifdef DISABLE_PARALLEL
    for (int x = 0; x < _width; ++x)
    {
        for (int y = 0; y < _height; ++y)
        {
            (*this)(x, y) = shader(x, y);
        }
    }
#else
    tbb::parallel_for(tbb::blocked_range2d<int>(0, _width, 0, _height), [&](const tbb::blocked_range2d<int>& r) {
        for (int x = r.rows().begin(); x < r.rows().end(); ++x)
        {
            for (int y = r.cols().begin(); y < r.cols().end(); ++y)
            {
                (*this)(x, y) = shader(x, y);
            }
        }
    });
#endif // DISABLE_PARALLEL
}

void rendertoy::Image::PixelShadeSSAA(const rendertoy::PixelShaderSSAA &shader, const int x_sample, const int y_sample)
{
// #define DISABLE_PARALLEL
#ifdef DISABLE_PARALLEL
        for (int x = 0; x < _width; ++x)
        {
            for (int y = 0; y < _height; ++y)
            {
                glm::vec4 contribution(0.0f);
                for (int xx = 0; xx < x_sample; ++xx)
                {
                    for (int yy = 0; yy < y_sample; ++yy)
                    {
                        glm::vec2 pixel_offset((static_cast<float>(xx) + 0.5f) / static_cast<float>(x_sample),
                                            (static_cast<float>(yy) + 0.5f) / static_cast<float>(x_sample));
                        glm::vec2 screen_coord((static_cast<float>(x) + pixel_offset.x) / static_cast<float>(_width), (static_cast<float>(y) + pixel_offset.y) / static_cast<float>(_height));
                        contribution += shader(screen_coord);
                    }
                }
                (*this)(x, y) = contribution * (1.0f / (static_cast<float>(x_sample) * static_cast<float>(y_sample)));
            }
        }
#else
    tbb::parallel_for(tbb::blocked_range2d<int>(0, _width, 0, _height), [&](const tbb::blocked_range2d<int>& r) {
        for (int x = r.rows().begin(); x < r.rows().end(); ++x)
        {
            for (int y = r.cols().begin(); y < r.cols().end(); ++y)
            {
                glm::vec4 contribution(0.0f);
                for (int xx = 0; xx < x_sample; ++xx)
                {
                    for (int yy = 0; yy < y_sample; ++yy)
                    {
                        glm::vec2 pixel_offset((static_cast<float>(xx) + 0.5f) / static_cast<float>(x_sample),
                                            (static_cast<float>(yy) + 0.5f) / static_cast<float>(x_sample));
                        glm::vec2 screen_coord((static_cast<float>(x) + pixel_offset.x) / static_cast<float>(_width), (static_cast<float>(y) + pixel_offset.y) / static_cast<float>(_height));
                        contribution += shader(screen_coord);
                    }
                }
                (*this)(x, y) = contribution * (1.0f / (static_cast<float>(x_sample) * static_cast<float>(y_sample)));
            }
        }
    });
#endif // DISABLE_PARALLEL
}

const rendertoy::Image rendertoy::Image::UpScale(const glm::float32 factor) const
{
    Image ret(static_cast<int>(_width * factor), static_cast<int>(_height * factor));
    PixelShader pixel_shader = [&](const int width, const int height) -> glm::vec4
    {
        return (*this)(static_cast<int>(width / factor), static_cast<int>(height / factor));
    };
    ret.PixelShade(pixel_shader);
    return ret;
}

const glm::vec4 rendertoy::Image::Avg() const
{
    glm::vec4 ret = glm::vec4(0.0f);
    for(int x = 0; x < _width; ++x)
    {
        for(int y = 0; y < _height; ++y)
        {
            ret += (*this)(x, y);
        }
    }
    ret /= static_cast<float>(_width) * static_cast<float>(_height);
    return ret;
}

rendertoy::Canvas::Canvas(int width, int height)
    : _width(width), _height(height)
{
}

const rendertoy::Image rendertoy::Canvas::ToImage() const
{
    Image ret(_width, _height, glm::vec4(0.0f));
    for (const rendertoy::Layer &layer : _layers)
    {
        for (int x = std::max(layer._position.x, 0); x < std::min(_width, layer._position.x + layer._image->width()); ++x)
        {
            for (int y = std::max(layer._position.y, 0); y < std::min(_height, layer._position.y + layer._image->height()); ++y)
            {
                switch (layer._mix_mode)
                {
                case MixMode::NORMAL:
                    ret(x, y) += layer._image->operator()(x - layer._position.x, y - layer._position.y);
                    break;
                case MixMode::DIFF:
                    ret(x, y) = glm::abs(ret(x, y) - layer._image->operator()(x - layer._position.x, y - layer._position.y));
                    break;
                case MixMode::DIFF_CLAMP:
                    ret(x, y) = glm::clamp(glm::abs(ret(x, y) - layer._image->operator()(x - layer._position.x, y - layer._position.y)), glm::vec4(0.0f), glm::vec4(1.0f));
                    break;
                case MixMode::MAX:
                    ret(x, y) = glm::max(ret(x, y), layer._image->operator()(x - layer._position.x, y - layer._position.y));
                    break;
                case MixMode::INVERT: {
                    auto color = layer._image->operator()(x - layer._position.x, y - layer._position.y);
                    if(color[3] != 0.0f)
                    {
                        ret(x, y) *= glm::vec4(-glm::vec3(1.0f), 1.0f);
                        ret(x, y) += glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
                    }
                    break;
                }
                case MixMode::NORMAL_CLAMP: {
                    ret(x, y) += layer._image->operator()(x - layer._position.x, y - layer._position.y);
                    ret(x, y) = glm::clamp(ret(x, y), glm::vec4(0.0f), glm::vec4(1.0f));
                    break;
                }
                default:
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
