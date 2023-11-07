add_executable(Executable main.cpp)
add_executable(Alpha alpha.cpp)

target_link_libraries(Executable PRIVATE RenderToy2)
target_include_directories(Executable PRIVATE ../)

target_link_libraries(Alpha PRIVATE RenderToy2)
target_include_directories(Alpha PRIVATE ../)

file(COPY alpha.obj alpha.png
     DESTINATION ${CMAKE_BINARY_DIR}/executable)
