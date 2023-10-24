#include <iostream>
#include <glm/glm.hpp>

#include "rendertoy.h"

bool Test(int RENDERTOY_FUNC_ARGUMENT_OUT out_integer)
{
    out_integer = 1;
    return true;
}

void say_hello(){
    std::cout << "Hello, from RenderToy2!\n";

    glm::mat3 mat{1.0f};
    Test(RENDERTOY_DISCARD_VARIABLE<int>);
}
