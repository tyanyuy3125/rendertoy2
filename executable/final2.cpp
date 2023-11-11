#include <iostream>
#include <memory>

#include "rendertoy.h"
#include "logger.h"
#include "principled.h"

using namespace rendertoy;

int main()
{
    INFO << "Welcome to the \'final2\' executable program of RenderToy2!" << std::endl;
    auto ret = ImportMeshFromFile("./final2.obj");
    INFO << "Import done." << std::endl;

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->objects().insert(scene->objects().end(), std::make_move_iterator(ret.begin()), std::make_move_iterator(ret.end()));

    std::shared_ptr<ISamplableColor> tex_white = std::make_shared<ColorTexture>(glm::vec4{1.0f});
    std::shared_ptr<ISamplableColor> tex_desert = std::make_shared<ImageTexture>("./desert.jpg");
    std::shared_ptr<ISamplableColor> tex_bones = std::make_shared<ImageTexture>("./bones.jpg");
    std::shared_ptr<ISamplableColor> tex_cactus = std::make_shared<ImageTexture>("./cactus.jpg");
    std::shared_ptr<ISamplableColor> tex_grass = std::make_shared<ImageTexture>("./desert_grass.png");

    std::shared_ptr<ISamplableNumerical> diffuse_roughness = std::make_shared<ConstantNumerical>(0.0f);
    std::shared_ptr<ISamplableNumerical> chair_roughness = std::make_shared<ConstantNumerical>(60.0f);

    std::shared_ptr<IMaterial> mat_desert = std::make_shared<DiffuseBSDF>(tex_desert, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_bones = std::make_shared<DiffuseBSDF>(tex_bones, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_cactus = std::make_shared<DiffuseBSDF>(tex_cactus, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_white = std::make_shared<DiffuseBSDF>(tex_white, diffuse_roughness);

    std::shared_ptr<IMaterial> mat_principled = std::make_shared<PrincipledBSDF>(
        tex_grass,
        std::make_shared<ConstantNumerical>(0.25f),
        std::make_shared<ConstantNumerical>(1.333f),
        std::make_shared<ConstantNumerical>(0.2f),
        std::make_shared<ConstantNumerical>(1.0f),
        std::make_shared<ConstantNumerical>(0.0f),
        std::make_shared<ConstantNumerical>(1.0f),
        std::make_shared<ConstantNumerical>(0.5f),
        std::make_shared<ConstantNumerical>(0.0f),
        std::make_shared<ConstantNumerical>(0.0f),
        std::make_shared<ConstantNumerical>(0.0f),
        true,
        std::make_shared<ConstantNumerical>(0.0f),
        std::make_shared<ConstantNumerical>(0.2f));

    scene->objects()[0]->mat() = mat_desert;
    scene->objects()[1]->mat() = mat_bones;
    scene->objects()[2]->mat() = mat_cactus;
    scene->objects()[3]->mat() = mat_principled;
    scene->objects()[4]->mat() = mat_desert;

    scene->inf_lights().push_back(std::make_shared<HDRILight>("./desert_morning.hdr"));

    scene->Init();
    INFO << "Scene inited." << std::endl;
    std::shared_ptr<ISamplableColor> hdr_bg = std::make_shared<ColorTexture>(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    scene->hdr_background() = hdr_bg;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3{1.2601f, 0.10336f, 1.2215f}, glm::vec3{-1.275, 0.4832f, -1.424f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::radians(21.0f), 16.0f / 9.0f);
    camera->lens_radius() = 0.01f;
    camera->focal_distasnce() = 1.93f;

    RenderConfig conf;
    conf.width = 1280;
    conf.height = 720;
    conf.camera = camera;
    conf.scene = scene;
    conf.x_sample = 4;
    conf.y_sample = 4;
    conf.spp = 32;
    conf.gamma = 2.2f;
    ProductionalRenderWork renderwork(conf);
    renderwork.Render();
    Image result = renderwork.GetResult(false);
#ifdef _WIN32
    result.Export("E:/test4.png");
#else
    result.Export("/Users/tyanyuy3125/Pictures/test.png");
#endif // _WIN32

    return 0;
}