#pragma once

#include "rendertoy_internal.h"

#include <glm/glm.hpp>

namespace rendertoy
{
    const glm::vec3 UniformSampleHemisphere();

    const float PowerHeuristic(int nf, float f_pdf, int ng, float g_pdf);
}