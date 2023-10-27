#include <iostream>

#include "rendertoy.h"

using namespace rendertoy;

int main()
{
    std::cout<<"Welcome to the executable program of RenderToy2!"<<std::endl;

    TestRenderer test_renderer(RenderConfig{.height = 64, .width = 64});
    test_renderer.Render();

    Image result = test_renderer.GetResult();
    result.Export("~/test.png");

    return 0;
}