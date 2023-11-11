#include <iostream>
#include <memory>

#include "rendertoy.h"
#include "logger.h"
#include "principled.h"

using namespace rendertoy;

int main()
{
    INFO << "Welcome to the \'final1\' executable program of RenderToy2!" << std::endl;
    auto ret = ImportMeshFromFile("./final1.obj");
    INFO << "Import done." << std::endl;

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->objects().insert(scene->objects().end(), std::make_move_iterator(ret.begin()), std::make_move_iterator(ret.end()));

    std::shared_ptr<ISamplableColor> tex_white = std::make_shared<ColorTexture>(glm::vec4{1.0f});
    std::shared_ptr<ISamplableColor> daisy_leaf = std::make_shared<ImageTexture>("./daisyleaf.png");
    std::shared_ptr<ISamplableColor> flowerdaisy = std::make_shared<ImageTexture>("./flowerdaisy.png");
    std::shared_ptr<ISamplableColor> plant05 = std::make_shared<ImageTexture>("./plant05.png");

    std::shared_ptr<ISamplableNumerical> diffuse_roughness = std::make_shared<ConstantNumerical>(0.0f);
    std::shared_ptr<ISamplableNumerical> chair_roughness = std::make_shared<ConstantNumerical>(60.0f);

    std::shared_ptr<IMaterial> mat_daisy_leaf = std::make_shared<DiffuseBSDF>(daisy_leaf, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_flowerdaisy = std::make_shared<DiffuseBSDF>(flowerdaisy, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_plant05 = std::make_shared<DiffuseBSDF>(plant05, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_white = std::make_shared<DiffuseBSDF>(tex_white, diffuse_roughness);

    std::shared_ptr<ISamplableNumerical> metal_roughness = std::make_shared<ConstantNumerical>(0.01f);
    std::shared_ptr<ISamplableColor> silver_eta = std::make_shared<ColorTexture>(glm::vec4(0.15943, 0.14512, 0.13547, 1.0f));
    std::shared_ptr<ISamplableColor> silver_k = std::make_shared<ColorTexture>(glm::vec4(3.92910, 3.19000, 2.38080, 1.0f));
    std::shared_ptr<IMaterial> mat_metal = std::make_shared<MetalBSDF>(tex_white, silver_eta, silver_k, metal_roughness, metal_roughness);

    std::shared_ptr<ISamplableColor> tex_wall = std::make_shared<ImageTexture>("./concrete.png");
    std::shared_ptr<IMaterial> mat_wall = std::make_shared<DiffuseBSDF>(tex_wall, diffuse_roughness);

    std::shared_ptr<ISamplableColor> tex_ground = std::make_shared<ImageTexture>("./concrete2.png");
    std::shared_ptr<ISamplableColor> tex_roughness = std::make_shared<ImageTexture>("./ground-roughness.png");
    std::shared_ptr<ISamplableNumerical> roughness_ground = std::make_shared<Brightness>(tex_roughness);
    std::shared_ptr<IMaterial> mat_ground = std::make_shared<DiffuseBSDF>(tex_ground, diffuse_roughness);

    std::shared_ptr<ISamplableColor> tex_chair = std::make_shared<ImageTexture>("./chair.jpg");
    std::shared_ptr<IMaterial> mat_chair = std::make_shared<DiffuseBSDF>(tex_chair, chair_roughness);

    std::shared_ptr<ISamplableNumerical> emissive_strength = std::make_shared<ConstantNumerical>(2.0f);
    std::shared_ptr<IMaterial> mat_emissive = std::make_shared<Emissive>(tex_white, emissive_strength);

    std::shared_ptr<IMaterial> mat_principled = std::make_shared<PrincipledBSDF>(
        tex_ground,
        std::make_shared<ConstantNumerical>(0.25f),
        std::make_shared<ConstantNumerical>(1.333f),
        roughness_ground,
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

    std::shared_ptr<IMaterial> mat_principled2 = std::make_shared<PrincipledBSDF>(
        tex_chair,
        std::make_shared<ConstantNumerical>(0.1f),
        std::make_shared<ConstantNumerical>(1.333f),
        std::make_shared<ConstantNumerical>(0.3f),
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

    scene->objects()[0]->mat() = mat_metal;
    scene->objects()[1]->mat() = mat_wall;
    scene->objects()[2]->mat() = mat_principled;
    scene->objects()[3]->mat() = mat_principled2;
    scene->objects()[4]->mat() = mat_white;
    scene->objects()[5]->mat() = mat_daisy_leaf;
    scene->objects()[6]->mat() = mat_wall;
    scene->objects()[7]->mat() = mat_emissive;
    scene->objects()[8]->mat() = mat_plant05;
    scene->objects()[9]->mat() = mat_flowerdaisy;

    scene->Init();
    INFO << "Scene inited." << std::endl;
    std::shared_ptr<ISamplableColor> hdr_bg = std::make_shared<ColorTexture>(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    scene->hdr_background() = hdr_bg;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3{-1.2511f, 0.87042f, 3.0f}, glm::vec3{-1.2511f, 0.97042f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::radians(37.0f), 16.0f / 9.0f);

    RenderConfig conf;
    conf.width = 1280;
    conf.height = 720;
    conf.camera = camera;
    conf.scene = scene;
    conf.x_sample = 4;
    conf.y_sample = 4;
    conf.spp = 32;
    conf.gamma = 2.2f;
    PathTracingRenderWork renderwork(conf);
    renderwork.Render();
    Image result = renderwork.GetResult(false);
#ifdef _WIN32
    result.Export("E:/test.png");
#else
    result.Export("/Users/tyanyuy3125/Pictures/test.png");
#endif // _WIN32

    return 0;
}