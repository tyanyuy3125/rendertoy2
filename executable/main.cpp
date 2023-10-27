#include <iostream>

#include "rendertoy.h"

using namespace rendertoy;

int main()
{
    std::cout<<"Welcome to the executable program of RenderToy2!"<<std::endl;

    TestRenderer test_renderer(RenderConfig{.width = 1920, .height = 1080});
    test_renderer.Render();

    Image result = test_renderer.GetResult();
    result.Export("E:/test.png");

    return 0;
}