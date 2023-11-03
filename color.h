#pragma once

#include <string>

#include "rendertoy_internal.h"

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
            "sRGB"};

    const float Luminance(const glm::vec3 &color);
}