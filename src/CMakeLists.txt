cmake_minimum_required(VERSION 2.4)
project(Q)


set(GRAPHICS_SOURCE_FILES
Graphics/Mesh.cpp
Graphics/Model.cpp
Graphics/Shader.cpp
Graphics/Renderer.cpp
Graphics/InstancedRenderer.cpp
Graphics/Texture.cpp
)

FILE(GLOB UI_SOURCE_FILES
UI/imgui*.cpp
UI/UI.cpp
UI/TextEditor.cpp
UI/ImGuizmo.cpp
)

set(SOURCE_FILES
Program.cpp
Platform/Platform.cpp
Metrics/CountDebugRegions.cpp
ResourceManager.cpp
Managers/EntityManager.cpp
Thirdparty/glad.c
${GRAPHICS_SOURCE_FILES}
${UI_SOURCE_FILES}
)

## Compile Link ##
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/Thirdparty)
link_directories(${PROJECT_SOURCE_DIR}/Thirdparty/libs)


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /nologo /Zi  /EHsc /wd4005 /DQDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /nologo /Zi  /EHsc")

add_executable(Q ${SOURCE_FILES})

target_link_libraries(Q 
                       opengl32 glfw3dll
                       debug zlibstaticd optimized zlibstatic
                       debug IrrXMLd optimized IrrXML
                       debug assimp-vc142-mtd optimized assimp-vc142-mt)


#### Copy Assets ####
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy-assets.cmake
)

add_dependencies(Q copy_assets)