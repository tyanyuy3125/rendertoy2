#include <iostream>
#include <memory>

#include "rendertoy.h"
#include "logger.h"
#include "principled.h"
#include "composition.h"

using namespace rendertoy;

int main()
{
    INFO << "Welcome to the \'voltest\' executable program of RenderToy2!" << std::endl;

    std::shared_ptr<ISamplableColor> tex_white = std::make_shared<ColorTexture>(glm::vec4{1.0f});
    std::shared_ptr<ISamplableNumerical> diffuse_roughness = std::make_shared<ConstantNumerical>(45.0f);
    std::shared_ptr<IMaterial> mat_white = std::make_shared<DiffuseBSDF>(tex_white, diffuse_roughness);

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->objects().push_back(std::make_shared<SDF>([](glm::vec3 coord) -> float
                                                     { return glm::length(coord) - 1.0f; },
                                                     [](glm::vec3 coord) -> glm::vec3
                                                     {
                                                         return glm::normalize(coord);
                                                     },
                                                     BBox(glm::vec3(-1.0f), glm::vec3(1.0f)),
                                                     glm::pi<float>() * 4.0f));
    // scene->inf_lights().push_back(std::make_shared<HDRILight>("./hdri.hdr"));
    scene->inf_lights().push_back(std::make_shared<HDRILight>("./hdri.hdr"));
    scene->Init();
    INFO << "Scene inited." << std::endl;
    scene->objects()[0]->mat() = mat_white;


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