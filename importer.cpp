#include <memory>

#include "importer.h"

const std::vector<std::unique_ptr<rendertoy::TriangleMesh>> rendertoy::ImportMeshFromFile(const std::string &path)
{
    std::vector<std::unique_ptr<TriangleMesh>> ret;

    Assimp::Importer importer;
    const auto scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        WARN << "No meshes found in file " << path << std::endl;
        return {};
    }

    INFO << "Found " << scene->mNumMeshes << " mesh(es) in file " << path << std::endl;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[i];
        auto vertices = mesh->mVertices;
        std::unique_ptr<TriangleMesh> tmp = std::make_unique<TriangleMesh>();
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
        {
            tmp->triangles.objects.push_back(std::make_unique<Triangle>(vertices[mesh->mFaces[j].mIndices[0]], vertices[mesh->mFaces[j].mIndices[1]], vertices[mesh->mFaces[j].mIndices[2]]));
        }
        INFO << "Mesh " << i << " has " << mesh->mNumFaces << " faces. Now constructing BVH." << std::endl;
        tmp->triangles.Construct();
        ret.push_back(std::move(tmp));
    }

    return ret;
}