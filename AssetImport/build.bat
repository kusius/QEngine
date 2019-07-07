@echo off

@REM We use pushd popd to make sure 
@REM all the compilation output goes into build
mkdir ..\build
pushd ..\build
cl  -Zi  ..\AssetImport\Program.cpp  ..\AssetImport\ResourceManager.cpp  ..\AssetImport\Thirdparty\glad.c ..\AssetImport\Graphics\*.cpp ^
opengl32.lib glfw3dll.lib zlibd.lib IrrXML.lib assimp-vc140-mt.lib
popd

