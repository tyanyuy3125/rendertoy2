#include <memory>
#include <OpenImageIO/imageio.h>

#include "importer.h"
#include "logger.h"
#include "composition.h"

#include <assimp/vector2.h>
#include <assimp/vector3.h>

const std::vector<std::unique_ptr<rendertoy::TriangleMesh>> rendertoy::ImportMeshFromFile(const std::string &path)
{
    std::vector<std::unique_ptr<TriangleMesh>> ret;

    Assimp::Importer importer;
    const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenBoundingBoxes);

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
        aiVector3D *uvs = mesh->mTextureCoords[0]; // TODO: UV information may not be stored in channel 0.
        std::unique_ptr<TriangleMesh> tmp = std::make_unique<TriangleMesh>();
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
        {
            tmp->triangles.objects.push_back(std::make_unique<Triangle>(vertices[mesh->mFaces[j].mIndices[0]], vertices[mesh->mFaces[j].mIndices[1]], vertices[mesh->mFaces[j].mIndices[2]], 
            aiVector2D(uvs[mesh->mFaces[j].mIndices[0]].x, uvs[mesh->mFaces[j].mIndices[0]].y), 
            aiVector2D(uvs[mesh->mFaces[j].mIndices[1]].x, uvs[mesh->mFaces[j].mIndices[1]].y), 
            aiVector2D(uvs[mesh->mFaces[j].mIndices[2]].x, uvs[mesh->mFaces[j].mIndices[2]].y)));
        }
        INFO << "Mesh " << i << " has " << mesh->mNumFaces << " faces. Now constructing BVH." << std::endl;
        auto aabb = mesh->mAABB;
        tmp->_bbox = BBox(glm::vec3(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z), glm::vec3(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z));
        tmp->triangles.Construct();
        ret.push_back(std::move(tmp));
    }

    return ret;
}

const rendertoy::Image rendertoy::ImportImageFromFile(const std::string &path)
{
    std::unique_ptr<OIIO::ImageInput> in = OIIO::ImageInput::open(path);
    if (!in)
    {
        CRIT << "Could not open image: " << path << std::endl;
        return Image(16, 16);
    }

    int width = in->spec().width;
    int height = in->spec().height;
    int channels = in->spec().nchannels;

    INFO << "Image opened. Resolution: " << width << "x" << height << ", channels: " << channels << std::endl;

    Image ret(width, height);
    in->read_image(0, 0, 0, 4, OIIO::TypeDesc::FLOAT, &ret._buffer[0]);
    return ret;
}
