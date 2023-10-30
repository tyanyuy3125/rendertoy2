#include "scene.h"

void rendertoy::Scene::Init()
{
    objects.Construct();
}

const bool rendertoy::Scene::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
{
    return objects.Intersect(origin, direction, intersect_info);
}
