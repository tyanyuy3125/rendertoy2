#pragma once

#define RENDERTOY_FUNC_ARGUMENT_OUT &

template <typename T>
T RENDERTOY_DISCARD_VARIABLE;

#define BUILD_NUMBER 385
#define BUILD_DATE "2023-11-01+15:38:22"

#define CLASS_METADATA_MARK(classname) \
    public: \
        virtual const char* GetClassName() const { return #classname; } \
    private: \

