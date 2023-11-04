# RENDERTOY2 (WIP)

As the final project of Advanced Computer Graphics (Fall 2023, IIIS, Tsinghua Univ. ), RenderToy2 is a completely refactored version of the first generation RenderToy.

RenderToy2 是清华大学交叉信息研究院2023年秋课程《高等计算机图形学》的期末大作业，也是初代 RenderToy 完全重构的迭代版本。

## Implemented

* BRDF Path Tracing
* Adaptive Sampling
* SAH-BVH
* MIS-DLS
* Lambertian BRDF
* Oren-Layar BRDF
* Fresnel
* Texture Mapping (Similar to pbrt and Cycles, use `ISamplable<T>` to abstract the texture type)
* Tone mapping
* Programmable shader to manipulate pixels on images
* Layer-based image composition
* Bitmap font watermark

## Not yet implemented
* BSDF
* Depth of field
* ...

## May be implemented
* LOD
* Bidirectional Path Tracing
    * Caustics
* GPU Acceleration
* ...

## Requirements
* glm
* OpenImageIO
* assimp
* tbb
* OpenImageDenoise
