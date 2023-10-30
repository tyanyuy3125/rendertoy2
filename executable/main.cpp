#include <iostream>
#include <memory>

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
    auto ret = ImportMeshFromFile("/Users/tyanyuy3125/cube.obj");
    INFO <<"Import done"<<std::endl;

    Scene scene;
    scene.objects.objects.push_back(std::make_unique<UVSphere>(glm::vec3(0.0f), 1.0f));
    scene.objects.objects.push_back(std::make_unique<UVSphere>(glm::vec3(10.0f, 0.0f, 0.0f), 2.0f));
    scene.objects.objects.push_back(std::make_unique<UVSphere>(glm::vec3(-5.0f, 0.0f, 0.0f), 1.0f));
    scene.Init();
    INFO << "Scene inited" << std::endl;

    return 0;
}