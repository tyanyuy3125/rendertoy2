add_executable(CornellBox cornellbox.cpp)
add_executable(Alpha alpha.cpp)
add_executable(Fog fog.cpp)

target_link_libraries(CornellBox PRIVATE RenderToy2)
target_include_directories(CornellBox PRIVATE ../)

target_link_libraries(Alpha PRIVATE RenderToy2)
target_include_directories(Alpha PRIVATE ../)

target_link_libraries(Fog PRIVATE RenderToy2)
target_include_directories(Fog PRIVATE ../)

file(COPY alpha.obj alpha.png lens_mask.png test.png fog.obj
     DESTINATION ${CMAKE_BINARY_DIR}/executable)
