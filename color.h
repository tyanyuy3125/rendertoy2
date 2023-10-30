#pragma once

#include <string>

namespace rendertoy
{
    enum class ColorSpace
    {
        LINEAR = 0,
        SRGB,
    };

    const static std::string oiio_color_space_string[] = 
    {
        "Linear",
        "sRGB"
    };

    template <std::size_t Sample>
    class Spectrum
    {

    };
}