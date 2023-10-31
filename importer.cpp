#include <memory>
#include <OpenImageIO/imageio.h>

#include "importer.h"
#include "logger.h"
#include "composition.h"

#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
        auto norms = mesh->mNormals; // TODO: exception handling.
        std::unique_ptr<TriangleMesh> tmp = std::make_unique<TriangleMesh>();
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
        {
            tmp->_triangles.objects.push_back(std::make_unique<Triangle>(vertices[mesh->mFaces[j].mIndices[0]], vertices[mesh->mFaces[j].mIndices[1]], vertices[mesh->mFaces[j].mIndices[2]],
                                                                        aiVector2D(uvs[mesh->mFaces[j].mIndices[0]].x, uvs[mesh->mFaces[j].mIndices[0]].y),
                                                                        aiVector2D(uvs[mesh->mFaces[j].mIndices[1]].x, uvs[mesh->mFaces[j].mIndices[1]].y),
                                                                        aiVector2D(uvs[mesh->mFaces[j].mIndices[2]].x, uvs[mesh->mFaces[j].mIndices[2]].y),
                                                                        norms[mesh->mFaces[j].mIndices[0]],
                                                                        norms[mesh->mFaces[j].mIndices[1]],
                                                                        norms[mesh->mFaces[j].mIndices[2]]));
        }
        INFO << "Mesh " << i << " has " << mesh->mNumFaces << " faces. Now constructing BVH." << std::endl;
        auto aabb = mesh->mAABB;
        tmp->_bbox = BBox(glm::vec3(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z), glm::vec3(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z));
        tmp->_triangles.Construct();
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
    if (channels == 4)
        in->read_image(0, 0, 0, 4, OIIO::TypeDesc::FLOAT, &ret._buffer[0]);
    else if (channels == 3)
    {
        std::vector<glm::vec3> temp_buffer(width * height);
        in->read_image(0, 0, 0, channels, OIIO::TypeDesc::FLOAT, &temp_buffer[0]);
        for(int i=0;i<width * height;++i)
        {
            ret._buffer[i] = glm::vec4(temp_buffer[i], 1.0f);
        }
    }
    else
    {
        CRIT << "Not implemented" << std::endl;
        return Image(16, 16);
    }
    return ret;
}
