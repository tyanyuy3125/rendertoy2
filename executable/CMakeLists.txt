add_executable(CornellBox cornellbox.cpp)
add_executable(Alpha alpha.cpp)
add_executable(Fog fog.cpp)
add_executable(HDRI hdri.cpp)
add_executable(Final1 final1.cpp)
add_executable(Final2 final2.cpp)

target_link_libraries(CornellBox PRIVATE RenderToy2)
target_include_directories(CornellBox PRIVATE ../)

target_link_libraries(Alpha PRIVATE RenderToy2)
target_include_directories(Alpha PRIVATE ../)

target_link_libraries(Fog PRIVATE RenderToy2)
target_include_directories(Fog PRIVATE ../)

target_link_libraries(HDRI PRIVATE RenderToy2)
target_include_directories(HDRI PRIVATE ../)

target_link_libraries(Final1 PRIVATE RenderToy2)
target_include_directories(Final1 PRIVATE ../)

target_link_libraries(Final2 PRIVATE RenderToy2)
target_include_directories(Final2 PRIVATE ../)

if(WIN32)
set(SOURCE_FILE cactus.jpg desert.jpg bones.jpg desert_morning.hdr desert_grass.png final2.obj alpha.obj alpha.png ground-roughness.png lens_mask.png test.png fog.obj hdri.obj hdri.hdr daisyleaf.png flowerdaisy.png plant05.png flora_test.obj final1.obj concrete.png concrete2.png chair.jpg pot.jpg pot_r.jpg)
file(COPY ${SOURCE_FILE}
     DESTINATION ${CMAKE_BINARY_DIR}/executable/Debug)
file(COPY ${SOURCE_FILE}
     DESTINATION ${CMAKE_BINARY_DIR}/executable/Release)
else()
file(COPY ${SOURCE_FILE}
     DESTINATION ${CMAKE_BINARY_DIR}/executable)
endif()
