message("Begin configuration...")
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET "x64-windows")
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
project(RenderToy2 VERSION 0.1.0 LANGUAGES C CXX)

find_package(Python3 COMPONENTS Interpreter)
add_custom_target(
    update_build_number
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/update_build_number.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Updating Build Number"
)

include(CTest)
enable_testing()

add_library(RenderToy2 STATIC primitive.h primitive.cpp
                              accelerate.h accelerate.cpp
                              intersectinfo.h intersectinfo.cpp
                              dotfont.h dotfont.cpp
                              rendertoy_internal.h
                              scene.h scene.cpp
                              composition.h composition.cpp
                              renderwork.h renderwork.cpp
                              importer.h importer.cpp
                              logger.h
                              camera.h camera.cpp
                              texture.h texture.cpp
                              curve.h curve.cpp
                              color.h color.cpp
                              material.h material.cpp
                              sampler.h sampler.cpp
                              light.h light.cpp
                              fresnel.h fresnel.cpp
                              microfacet.h microfacet.cpp)

add_dependencies(RenderToy2 update_build_number)

find_package(glm REQUIRED)
target_link_libraries(RenderToy2 PUBLIC glm::glm)
find_package(OpenImageIO REQUIRED)
target_link_libraries(RenderToy2 PRIVATE OpenImageIO::OpenImageIO)
find_package(assimp REQUIRED)
if(WIN32)
    target_link_libraries(RenderToy2 PRIVATE assimp::assimp)
else()
    target_link_libraries(RenderToy2 PRIVATE assimp)
endif()
find_package(tbb REQUIRED)
if(WIN32)
    target_link_libraries(RenderToy2 PRIVATE TBB::tbb TBB::tbbmalloc)
else()
    target_link_libraries(RenderToy2 PRIVATE tbb)
endif()
# find_package(OpenImageDenoise REQUIRED)
# target_link_libraries(RenderToy2 PRIVATE OpenImageDenoise)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(./executable)
