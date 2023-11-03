#pragma once

#include <vector>
#include <string>
#include <memory>

#include "rendertoy_internal.h"

namespace rendertoy
{
    const std::vector<std::shared_ptr<TriangleMesh>> ImportMeshFromFile(const std::string &path);

    const Image ImportImageFromFile(const std::string &path);
}