#pragma once

#define RENDERTOY_FUNC_ARGUMENT_OUT &

template <typename T>
T RENDERTOY_DISCARD_VARIABLE;

#define BUILD_NUMBER 176
#define BUILD_DATE "2023-10-31+10:36:42"

#define CLASS_METADATA_MARK(classname) \
    public: \
        virtual const char* GetClassName() const { return #classname; } \
    private: \

