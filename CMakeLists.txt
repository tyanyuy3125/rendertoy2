message("Begin configuration...")
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET "x64-windows")
    set(PKG_CONFIG_EXECUTABLE "E:/pkg-config-lite-0.28-1/bin/pkg-config.exe")
elseif(UNIX)
    message(FATAL_ERROR "Not implemented")
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

cmake_minimum_required(VERSION 3.0.0)
project(RenderToy2 VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_library(RenderToy2 RenderToy2.cpp primitive.h bbox.cpp bbox.h intersectinfo.h rendertoy.h)

find_package(glm REQUIRED)
target_link_libraries(RenderToy2 PRIVATE glm::glm)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(CTest REQUIRED)
add_executable(RenderToyTests test_rendertoy.cpp)
target_link_libraries(RenderToyTests PRIVATE RenderToy2 ctest)
