#include "accelerate.h"

#include <algorithm>
#include <cmath>
#include <optional>

const bool rendertoy::BBox::Intersect(const glm::vec3 &origin, const glm::vec3 &direction, float &t) const
{
    glm::vec3 inv_direction = 1.0f / direction;

    glm::vec3 t1 = (_pmin - origin) * inv_direction;
    glm::vec3 t2 = (_pmax - origin) * inv_direction;

    glm::vec3 tmin = glm::min(t1, t2);
    glm::vec3 tmax = glm::max(t1, t2);

    // float t_enter = glm::max(tmin.x, glm::max(tmin.y, tmin.z));
    // float t_exit = glm::min(tmax.x, glm::min(tmax.y, tmax.z));
    float t_enter = glm::compMax(tmin);
    float t_exit = glm::compMin(tmax);

    if(t_exit >= t_enter && t_exit >= 0.0f)
    {
        if(t_enter < 0)
        {
            t = t_exit;
        }
        else
        {
            t = t_enter;
        }
        return true;
    }
    else
    {
        return false;
    }
}

const glm::vec3 rendertoy::BBox::GetCenter() const
{
    return (_pmin + _pmax) * 0.5f;
}

void rendertoy::BBox::Union(const BBox &a)
{
    for(int i=0;i<3;++i)
    {
        _pmin[i] = std::min(_pmin[i], a._pmin[i]);
        _pmax[i] = std::max(_pmax[i], a._pmax[i]);  
    }
}

const int rendertoy::BBox::GetLongestAxis() const
{
    auto diagonal = _pmax - _pmin;
    int ret = 0;
    auto max_val = diagonal[0];
    for (int i = 1; i < 3; ++i)
    {
        if (diagonal[i] > max_val)
        {
            ret = i;
            max_val = diagonal[i];
        }
    }
    return ret;
}
