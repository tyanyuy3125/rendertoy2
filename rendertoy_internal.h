#pragma once

#include <string>
#include <glm/glm.hpp>

#define RENDERTOY_FUNC_ARGUMENT_OUT &

template <typename T>
T RENDERTOY_DISCARD_VARIABLE;

#define BUILD_NUMBER 568
#define BUILD_DATE "2023-11-03+14:38:33"

#define CLASS_METADATA_MARK(classname) \
    public: \
        virtual const char* GetClassName() const { return #classname; } \
    private: \

#define MATERIAL_SOCKET(name, isamplable_type)                             \
protected:                                                                 \
    std::shared_ptr<ISamplable##isamplable_type> _##name; \
                                                                           \
public:                                                                    \
    const std::shared_ptr<ISamplable##isamplable_type> &name() const       \
    {                                                                      \
        return _##name;                                                    \
    }                                                                      \
    std::shared_ptr<ISamplable##isamplable_type> &name()                   \
    {                                                                      \
        return _##name;                                                    \
    }                                                                      \
                                                                           \
private:

namespace rendertoy
{
    class Camera;
    class Image;
    class IMaterial;
    class IntersectInfo;
    template <typename T = glm::vec4>
    class ISamplable;
    typedef ISamplable<glm::vec4> ISamplableColor;
    typedef ISamplable<float> ISamplableNumerical;
    class Primitive;
    class Scene;
    class TriangleMesh;
}
