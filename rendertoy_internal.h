#pragma once

#define RENDERTOY_FUNC_ARGUMENT_OUT &

template <typename T>
T RENDERTOY_DISCARD_VARIABLE;

#define BUILD_NUMBER 120
#define BUILD_DATE "2023-10-31+00:03:44"

#define CLASS_METADATA_MARK(classname) \
    public: \
        virtual const char* GetClassName() const { return #classname; } \
    private: \

