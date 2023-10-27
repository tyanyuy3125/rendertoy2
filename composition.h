#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <functional>

namespace rendertoy
{
    typedef std::function<glm::vec3(const int, const int)> PixelShader;

    class Image
    {
    private:
        std::vector<glm::vec3> _buffer;
        int _width;
        int _height;

    public:
        const glm::vec3 &operator()(const int x, const int y) const;
        glm::vec3 &operator()(const int x, const int y);

        const int width() const;
        const int height() const;

        Image() = delete;
        Image(const int width, const int height);

        void Export(const std::string &filename) const;

        void PixelShade(const PixelShader &shader);
    };
}