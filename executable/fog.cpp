#include <iostream>
#include <memory>

#include "rendertoy.h"
#include "logger.h"
#include "principled.h"

using namespace rendertoy;

int main()
{
    INFO<<"Welcome to the \'alpha\' executable program of RenderToy2!"<<std::endl;
    auto ret = ImportMeshFromFile("./fog.obj");
    INFO <<"Import done."<<std::endl;

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->objects().insert(scene->objects().end(), std::make_move_iterator(ret.begin()), std::make_move_iterator(ret.end()));

    std::shared_ptr<ISamplableColor> tex_white = std::make_shared<ColorTexture>(glm::vec4{1.0f});
    std::shared_ptr<ISamplableColor> tex_alpha = std::make_shared<ImageTexture>("./alpha.png");
    tex_alpha->SetSampleMethod(SampleMethod::BILINEAR);
    std::shared_ptr<ISamplableColor> tex_black = std::make_shared<ColorTexture>(glm::vec4{0.0f});
    std::shared_ptr<ISamplableColor> tex_R = std::make_shared<ColorTexture>(glm::vec4{0.3f});
    std::shared_ptr<ISamplableColor> tex_red = std::make_shared<ColorTexture>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    std::shared_ptr<ISamplableColor> tex_green = std::make_shared<ColorTexture>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    std::shared_ptr<ISamplableNumerical> emissive_strength = std::make_shared<ConstantNumerical>(10.0f);
    std::shared_ptr<ISamplableNumerical> diffuse_roughness = std::make_shared<ConstantNumerical>(45.0f);
    std::shared_ptr<ISamplableNumerical> metal_roughness = std::make_shared<ConstantNumerical>(0.1f);
    std::shared_ptr<IMaterial> mat_white = std::make_shared<DiffuseBSDF>(tex_white, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_alpha = std::make_shared<DiffuseBSDF>(tex_alpha, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_red = std::make_shared<DiffuseBSDF>(tex_red, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_green = std::make_shared<DiffuseBSDF>(tex_green, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_emissive = std::make_shared<Emissive>(tex_white, emissive_strength);
    std::shared_ptr<IMaterial> mat_specular = std::make_shared<SpecularBSDF>(tex_white);
    std::shared_ptr<ISamplableColor> copper_eta = std::make_shared<ColorTexture>(glm::vec4(0.27105f, 0.67693f, 1.31640f, 1.0f));
    std::shared_ptr<ISamplableColor> copper_k = std::make_shared<ColorTexture>(glm::vec4(3.60920f, 2.62480f, 2.29210f, 1.0f));
    std::shared_ptr<ISamplableNumerical> water_eta = std::make_shared<ConstantNumerical>(1.333f);
    std::shared_ptr<IMaterial> mat_metal = std::make_shared<MetalBSDF>(tex_white, copper_eta, copper_k, metal_roughness, metal_roughness);
    std::shared_ptr<IMaterial> mat_glass = std::make_shared<RefractionBSDF>(tex_R, tex_white, water_eta);
    std::shared_ptr<ISamplableNumerical> glass_roughness = std::make_shared<ConstantNumerical>(0.0001f);
    std::shared_ptr<IMaterial> mat_frosted_glass = std::make_shared<RefractionBSDF>(tex_white, tex_white, water_eta, glass_roughness, glass_roughness);
    // std::shared_ptr<IMaterial> mat_metal;
    scene->objects()[0]->mat() = mat_white;
    // scene->objects()[0]->mat() = mat_emissive;
    scene->lights().push_back(std::make_shared<DeltaLight>(glm::vec3(1.0f), 100.0f, glm::vec3(0.0f, 0.0f, -1.0f)));

    scene->Init();
    INFO << "Scene inited." << std::endl;
    std::shared_ptr<ISamplableColor> hdr_bg = std::make_shared<ColorTexture>(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    scene->hdr_background() = hdr_bg;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3{8.0f, 0.0f, 8.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::radians(45.0f), 16.0f / 9.0f);

    RenderConfig conf;
    conf.width = 1280;
    conf.height = 720;
    conf.camera = camera;
    conf.scene = scene;
    conf.x_sample = 4;
    conf.y_sample = 4;
    conf.spp = 8;
    conf.gamma = 2.4f;
    ProductionalRenderWork renderwork(conf);
    renderwork.Render();
    Image result = renderwork.GetResult(true);
    #ifdef _WIN32
    result.Export("E:/test.png");
    #else
    result.Export("/Users/tyanyuy3125/Pictures/test.png");
    #endif // _WIN32

    return 0;
}