@echo off
set SOURCE_DIR="..\src"
@REM We use pushd popd to make sure 
@REM all the compilation output goes into build
mkdir ..\build
pushd ..\build
cl  -Zi  %SOURCE_DIR%\Program.cpp  %SOURCE_DIR%\ResourceManager.cpp  %SOURCE_DIR%\Thirdparty\glad.c %SOURCE_DIR%\Graphics\*.cpp ^
opengl32.lib glfw3dll.lib zlibd.lib IrrXML.lib assimp-vc140-mt.lib
popd

xcopy Shaders ..\build\Shaders /Y
