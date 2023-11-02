#include <iostream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

#include "rendertoy.h"
#include "logger.h"

using namespace rendertoy;

int main()
{
    INFO<<"Welcome to the executable program of RenderToy2!"<<std::endl;
    
    // TestRenderWork test_renderer(RenderConfig{.width = 1920, .height = 1080});
    // test_renderer.Render();

    // Image result = test_renderer.GetResult();
    // result.Export("/Users/tyanyuy3125/Pictures/test.png");
    #ifdef _WIN32
    auto ret = ImportMeshFromFile("E:/cornellbox.obj");
    #else
    auto ret = ImportMeshFromFile("/Users/tyanyuy3125/cornellbox.obj");
    // auto ret = ImportMeshFromFile("/Users/tyanyuy3125/monkey.obj");
    #endif // _WIN32
    INFO <<"Import done."<<std::endl;

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->objects().insert(scene->objects().end(), std::make_move_iterator(ret.begin()), std::make_move_iterator(ret.end()));
    // scene->objects().push_back(std::make_shared<UVSphere>(glm::vec3(0.0f), 1.0f));
    // scene->objects().push_back(std::make_shared<UVSphere>(glm::vec3(10.0f, 0.0f, 0.0f), 2.0f));
    // scene->objects().push_back(std::make_shared<UVSphere>(glm::vec3(-5.0f, 0.0f, 0.0f), 1.0f));
    // scene->objects().push_back(std::make_shared<UVSphere>(glm::vec3(-5.0f, -5.0f, 0.0f), 1.0f));
    // scene->objects().push_back(std::make_shared<UVSphere>(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f));

    std::shared_ptr<ISamplableColor> tex_white = std::make_shared<ColorTexture>(glm::vec4{1.0f});
    std::shared_ptr<ISamplableColor> tex_red = std::make_shared<ColorTexture>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    std::shared_ptr<ISamplableColor> tex_green = std::make_shared<ColorTexture>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    std::shared_ptr<ISamplableNumerical> emissive_strength = std::make_shared<ConstantNumerical>(10.0f);
    std::shared_ptr<IMaterial> mat_white = std::make_shared<DiffuseBSDF>();
    std::shared_ptr<IMaterial> mat_red = std::make_shared<DiffuseBSDF>(tex_red);
    std::shared_ptr<IMaterial> mat_green = std::make_shared<DiffuseBSDF>(tex_green);
    std::shared_ptr<IMaterial> mat_emissive = std::make_shared<Emissive>(tex_white, emissive_strength);
    scene->objects()[0]->mat() = mat_white;
    scene->objects()[1]->mat() = mat_white;
    scene->objects()[2]->mat() = mat_white;
    scene->objects()[3]->mat() = mat_emissive;
    scene->objects()[4]->mat() = mat_red;
    scene->objects()[5]->mat() = mat_green;

    scene->Init();
    INFO << "Scene inited." << std::endl;

    // std::shared_ptr<ISamplable> hdr_bg = std::make_shared<ImageTexture>("/Applications/Blender.app/Contents/Resources/3.6/datafiles/studiolights/world/sunrise.exr");
#ifdef _WIN32
    // std::shared_ptr<ISamplableColor> hdr_bg = std::make_shared<ImageTexture>("E:/rooftop_night_1k.hdr");
#else
    // std::shared_ptr<ISamplableColor> hdr_bg = std::make_shared<ImageTexture>("/Users/tyanyuy3125/Desktop/farm_sunset_1k.hdr");
#endif
    std::shared_ptr<ISamplableColor> hdr_bg = std::make_shared<ColorTexture>(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    hdr_bg->SetSampleMethod(SampleMethod::BILINEAR);
    scene->hdr_background() = hdr_bg;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3{0.0f, 1.0f, 4.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::radians(45.0f), 16.0f / 9.0f);

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
    Image result = renderwork.GetResult(true);
    #ifdef _WIN32
    result.Export("E:/test.png");
    #else
    result.Export("/Users/tyanyuy3125/Pictures/test.png");
    #endif // _WIN32

    // auto hdr_bg_raw_ptr = dynamic_cast<ImageTexture *>(hdr_bg.get());
    // CRIT<<hdr_bg_raw_ptr->_image(313,217)<<std::endl;
    // CRIT<<hdr_bg_raw_ptr->_image(50,50)<<std::endl;
    // PixelShader ps5 = [&](const int x, const int y) -> glm::vec4
    // {
    //     return glm::vec4(glm::vec3(hdr_bg_raw_ptr->_image(x, y)) * 1e8f, 1.0f);
    // };
    // hdr_bg_raw_ptr->_image.PixelShade(ps5);
    // hdr_bg_raw_ptr->_image.Export("/Users/tyanyuy3125/test2.exr");

    return 0;
}