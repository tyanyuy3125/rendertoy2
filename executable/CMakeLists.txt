add_executable(CornellBox cornellbox.cpp)
add_executable(Alpha alpha.cpp)

target_link_libraries(CornellBox PRIVATE RenderToy2)
target_include_directories(CornellBox PRIVATE ../)

target_link_libraries(Alpha PRIVATE RenderToy2)
target_include_directories(Alpha PRIVATE ../)

file(COPY alpha.obj alpha.png lens_mask.png test.png
     DESTINATION ${CMAKE_BINARY_DIR}/executable)
