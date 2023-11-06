# RENDERTOY2 (WIP)

As the final project of Advanced Computer Graphics (Fall 2023, IIIS, Tsinghua Univ. ), RenderToy2 is a completely refactored version of the first generation RenderToy.

RenderToy2 是清华大学交叉信息研究院2023年秋课程《高等计算机图形学》的期末大作业，也是初代 RenderToy 完全重构的迭代版本。

## Implemented

* BSDF Path Tracing
* Principled BSDF
* Adaptive Sampling
* SAH-BVH
* MIS-DLS
* Power Light Sampling (Using alias sampling)
* Lambertian BRDF
* Oren-Layar BRDF
* Specular BRDF
* Transmissive BSDF
* Fresnel
* Beckmann microfacet distribution
* Texture Mapping (Similar to pbrt and Cycles, use `ISamplable<T>` to abstract the texture type)
* Tone mapping
* Programmable shader to manipulate pixels on images
* Layer-based image composition
* Bitmap font watermark

## Not yet implemented
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

## License
This project is a course assignment and is not for profit. I would not license it, use it at your own risk.


Part of the PBR-related code is borrowed from pbrt-v3 & pbrt-v4. pbrt license:
```
pbrt source code is Copyright(c) 1998-2017
Matt Pharr, Greg Humphreys, and Wenzel Jakob.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
