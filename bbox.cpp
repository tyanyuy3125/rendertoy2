#include "bbox.h"


const glm::vec3 rendertoy::BBox::GetCenter() const
{
    return (_pmin + _pmax) * 0.5f;
}