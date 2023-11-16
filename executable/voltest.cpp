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

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    // scene->inf_lights().push_back(std::make_shared<HDRILight>("./hdri.hdr"));
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
    conf.spp = 1;
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