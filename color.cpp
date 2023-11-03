#include "color.h"

const float rendertoy::Luminance(const glm::vec3 &color)
{
    return 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
}