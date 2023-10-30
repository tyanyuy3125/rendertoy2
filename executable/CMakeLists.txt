add_executable(Executable main.cpp)

target_link_libraries(Executable PRIVATE RenderToy2)
target_include_directories(Executable PRIVATE ../)
