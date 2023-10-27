add_executable(Executable main.cpp)

target_link_libraries(Executable PRIVATE RenderToy2)
target_include_directories(Executable PRIVATE ../)

# find_package(OpenImageIO REQUIRED)
# target_include_directories(Executable PRIVATE ${OPENIMAGEIO_INCLUDE_DIRS})
# target_link_libraries(Executable PRIVATE ${OPENIMAGEIO_LIBRARIES})
