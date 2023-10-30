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
    #ifdef _WIN32
    #else
    auto ret = ImportMeshFromFile("/Users/tyanyuy3125/cube.obj");
    #endif // _WIN32
    INFO <<"Import done"<<std::endl;

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->objects().insert(scene->objects().end(), std::make_move_iterator(ret.begin()), std::make_move_iterator(ret.end()));
    // scene->objects().push_back(std::make_unique<UVSphere>(glm::vec3(0.0f), 1.0f));
    // scene->objects().push_back(std::make_unique<UVSphere>(glm::vec3(10.0f, 0.0f, 0.0f), 2.0f));
    // scene->objects().push_back(std::make_unique<UVSphere>(glm::vec3(-5.0f, 0.0f, 0.0f), 1.0f));
    // scene->objects().push_back(std::make_unique<UVSphere>(glm::vec3(-5.0f, -5.0f, 0.0f), 1.0f));
    // scene->objects().push_back(std::make_unique<UVSphere>(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f));
    scene->Init();
    INFO << "Scene inited" << std::endl;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 10.0f), glm::mat3(1.0f), glm::radians(90.0f), 16.0f / 9.0f);
    glm::vec3 origin, direction;
    camera->SpawnRay({1.0f, 1.0f}, origin, direction);
    INFO << origin << direction << std::endl;

    RenderConfig conf;
    conf.camera = camera;
    conf.scene = scene;
    NormalRenderWork renderwork(conf);
    renderwork.Render();
    Image result = renderwork.GetResult(true);
    #ifdef _WIN32
    result.Export("E:/test.png");
    #else
    result.Export("/Users/tyanyuy3125/Pictures/test.png");
    #endif // _WIN32



    return 0;
}