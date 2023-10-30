#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>

#include "rendertoy_internal.h"
#include "primitive.h"
#include "logger.h"
#include "composition.h"

namespace rendertoy
{
    const std::vector<std::unique_ptr<TriangleMesh>> ImportMeshFromFile(const std::string &path);

    const Image ImportImageFromFile(const std::string &path);
}