cmake_minimum_required(VERSION 2.4)
project(Q)

set(GRAPHICS_SOURCE_FILES
Graphics/Mesh.cpp
Graphics/Model.cpp
Graphics/Shader.cpp
Graphics/SpriteRenderer.cpp
Graphics/Texture.cpp
)

set(SOURCE_FILES
Program.cpp
ResourceManager.cpp
Thirdparty/glad.c
${GRAPHICS_SOURCE_FILES}
)

## Compile Link ##
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/Thirdparty)
link_directories(${PROJECT_SOURCE_DIR}/Thirdparty/libs)


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /nologo /Zi  /EHsc /wd4005")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /nologo /Zi  /EHsc /wd4005")

add_executable(Q ${SOURCE_FILES})

target_link_libraries(Q 
                       opengl32 glfw3dll 
                       debug zlibstaticd optimized zlibstatic
                       debug IrrXMLd optimized IrrXML
                       debug assimp-vc142-mtd optimized assimp-vc142-mt)



#### Copy Assets folder ####
add_custom_target(copy_resources ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${PROJECT_SOURCE_DIR}/Assets" ${CMAKE_BINARY_DIR}/Assets
)