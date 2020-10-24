@echo off
set PROJECT_DIR=D:\work\QEngine
set SRCDIR=%PROJECT_DIR%\src
set PROJECT_BUILD_DIR=%PROJECT_DIR%\build

set PROJECT_LIB_DIRECTORIES=%SRCDIR%\Thirdparty\libs

:: MSVC reads these env variables by default to search for objects, 
:: libraries and include directories
set LIB=%LIB%;%PROJECT_LIB_DIRECTORIES%
set INCLUDE=%INCLUDE%;%SRCDIR%;%SRCDIR%\Thirdparty


@REM Debug configuration
set DEBUG_COMPILER_FLAGS=/nologo /Zi /EHsc /wd4005 /DQDEBUG
set GRAPHICS_SOURCE_FILES=^
 %SRCDIR%\Graphics\Mesh.cpp^
 %SRCDIR%\Graphics\Model.cpp^
 %SRCDIR%\Graphics\Shader.cpp^
 %SRCDIR%\Graphics\SpriteRenderer.cpp^
 %SRCDIR%\Graphics\Texture.cpp^

set UI_SOURCE_FILES=UI\imgui*.cpp^
 %SRCDIR%\UI\UI.cpp^
 %SRCDIR%\UI\TextEditor.cpp

set SOURCE_FILES=^
 %SRCDIR%\Program.cpp^
 %SRCDIR%\Platform\Platform.cpp^
 %SRCDIR%\Metrics\CountDebugRegions.cpp^
 %SRCDIR%\ResourceManager.cpp^
 %SRCDIR%\Managers\EntityManager.cpp^
 %SRCDIR%\Thirdparty\glad.c^
 %GRAPHICS_SOURCE_FILES%^
 %UI_SOURCE_FILES%

set DEBUG_LINK_LIBRARIES=opengl32^
 glfw3dll^
 zlibstaticd^
 IrrXMLd^
 assimp-vc142-mtd^
 
:: We use pushd popd to make sure 
:: all the compilation output goes into build
mkdir %PROJECT_BUILD_DIR%\bat_test
pushd %PROJECT_BUILD_DIR%\bat_test

cl %DEBUG_COMPILER_FLAGS% %SOURCE_FILES% /link /out:QEngine.exe
del *.obj

popd

@REM xcopy Shaders ..\build\Shaders /Y
