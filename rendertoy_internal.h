#pragma once

#define RENDERTOY_FUNC_ARGUMENT_OUT &

template <typename T>
T RENDERTOY_DISCARD_VARIABLE;

#define BUILD_NUMBER 259
#define BUILD_DATE "2023-10-31+15:41:31"

#define CLASS_METADATA_MARK(classname) \
    public: \
        virtual const char* GetClassName() const { return #classname; } \
    private: \

