#include <iostream>
#include <memory>

#include "rendertoy.h"
#include "logger.h"
#include "principled.h"
#include "composition.h"

using namespace rendertoy;

int main()
{
    INFO << "Welcome to the \'hdri\' executable program of RenderToy2!" << std::endl;
    auto ret = ImportMeshFromFile("./hdri.obj");
    INFO << "Import done." << std::endl;

    // Image image(ImportImageFromFile("./hdri.hdr"));
    // image = image.NextMipMap();
    // image = image.NextMipMap();
    // image = image.NextMipMap();
    // image = img3;
    // img3.Export("/Users/tyanyuy3125/Pictures/test1.png");
    // float weights[image.width() * image.height()];
    // for (int y = 0; y < image.height(); ++y)
    // {
    //     for (int x = 0; x < image.width(); ++x)
    //     {
    //         weights[y * image.width() + x] = Luminance(image(x, y));
    //     }
    // }
    // std::shared_ptr<OIIO::ImageOutput> out = OIIO::ImageOutput::create("/Users/tyanyuy3125/Pictures/test.png");
    // OIIO::ImageSpec spec(16, 16, 1, OIIO::TypeDesc::FLOAT);
    // out->open("/Users/tyanyuy3125/Pictures/test.png", spec);
    // out->write_image(OIIO::TypeDesc::FLOAT, &weights[0]);
    // out->close();
    // Distribution2D distrib(&weights[0], image.width(), image.height());
    // Image result(16, 16, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    // for (int i = 0; i < 64; ++i)
    // {
    //     float pdf;
    //     glm::vec2 spt = distrib.SampleContinuous(glm::vec2(glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f)), &pdf);
    //     result(int(spt.x * 16), int(spt.y * 16)) += glm::vec4(glm::vec3(0.1f), 0.0f);
    // }
    // result.Export("/Users/tyanyuy3125/Pictures/test3.exr");

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->objects().insert(scene->objects().end(), std::make_move_iterator(ret.begin()), std::make_move_iterator(ret.end()));
    scene->inf_lights().push_back(std::make_shared<HDRILight>("./hdri.hdr"));

    std::shared_ptr<ISamplableColor> tex_white = std::make_shared<ColorTexture>(glm::vec4{1.0f});
    std::shared_ptr<ISamplableColor> tex_alpha = std::make_shared<ImageTexture>("./alpha.png");
    tex_alpha->SetSampleMethod(SampleMethod::BILINEAR);
    std::shared_ptr<ISamplableColor> tex_black = std::make_shared<ColorTexture>(glm::vec4{0.0f});
    std::shared_ptr<ISamplableColor> tex_R = std::make_shared<ColorTexture>(glm::vec4{0.3f});
    std::shared_ptr<ISamplableColor> tex_red = std::make_shared<ColorTexture>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    std::shared_ptr<ISamplableColor> tex_green = std::make_shared<ColorTexture>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    std::shared_ptr<ISamplableNumerical> emissive_strength = std::make_shared<ConstantNumerical>(10.0f);
    std::shared_ptr<ISamplableNumerical> diffuse_roughness = std::make_shared<ConstantNumerical>(45.0f);
    std::shared_ptr<ISamplableNumerical> metal_roughness = std::make_shared<ConstantNumerical>(0.01f);
    std::shared_ptr<ISamplableNumerical> metal_roughness_2 = std::make_shared<ConstantNumerical>(0.01f);
    std::shared_ptr<IMaterial> mat_white = std::make_shared<DiffuseBSDF>(tex_white, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_alpha = std::make_shared<DiffuseBSDF>(tex_alpha, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_red = std::make_shared<DiffuseBSDF>(tex_red, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_green = std::make_shared<DiffuseBSDF>(tex_green, diffuse_roughness);
    std::shared_ptr<IMaterial> mat_emissive = std::make_shared<Emissive>(tex_white, emissive_strength);
    std::shared_ptr<IMaterial> mat_specular = std::make_shared<SpecularBSDF>(tex_white);
    std::shared_ptr<ISamplableColor> copper_eta = std::make_shared<ColorTexture>(glm::vec4(0.27105f, 0.67693f, 1.31640f, 1.0f));
    std::shared_ptr<ISamplableColor> copper_k = std::make_shared<ColorTexture>(glm::vec4(3.60920f, 2.62480f, 2.29210f, 1.0f));
    std::shared_ptr<ISamplableColor> silver_eta = std::make_shared<ColorTexture>(glm::vec4(0.15943, 0.14512, 0.13547, 1.0f));
    std::shared_ptr<ISamplableColor> silver_k = std::make_shared<ColorTexture>(glm::vec4(3.92910, 3.19000, 2.38080, 1.0f));
    std::shared_ptr<ISamplableNumerical> water_eta = std::make_shared<ConstantNumerical>(1.333f);
    std::shared_ptr<IMaterial> mat_metal = std::make_shared<MetalBSDF>(tex_white, silver_eta, silver_k, metal_roughness, metal_roughness_2);
    std::shared_ptr<IMaterial> mat_glass = std::make_shared<RefractionBSDF>(tex_R, tex_white, water_eta);
    std::shared_ptr<ISamplableNumerical> glass_roughness = std::make_shared<ConstantNumerical>(0.0001f);
    std::shared_ptr<IMaterial> mat_frosted_glass = std::make_shared<RefractionBSDF>(tex_white, tex_white, water_eta, glass_roughness, glass_roughness);
    scene->objects()[0]->mat() = mat_metal;
    scene->objects()[1]->mat() = mat_white;
    scene->Init();
    INFO << "Scene inited." << std::endl;

    std::shared_ptr<ISamplableColor> hdr_bg = std::make_shared<ColorTexture>(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    scene->hdr_background() = hdr_bg;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3{-4.0f, 1.0f, 4.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::radians(45.0f), 16.0f / 9.0f);

    RenderConfig conf;
    conf.width = 1280;
    conf.height = 720;
    conf.camera = camera;
    conf.scene = scene;
    conf.x_sample = 4;
    conf.y_sample = 4;
    conf.spp = 4;
    conf.gamma = 2.4f;
    PathTracingRenderWork renderwork(conf);
    renderwork.Render();
    Image result = renderwork.GetResult(true);
#ifdef _WIN32
    result.Export("E:/test.png");
#else
    result.Export("/Users/tyanyuy3125/Pictures/test.png");
#endif // _WIN32

    return 0;
}