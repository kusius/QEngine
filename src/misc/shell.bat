@echo off 
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
set path=%cd%\misc;%path%
set include=c:\work\learnopengl\AssetImport\Thirdparty;%include%
set lib=c:\work\learnopengl\AssetImport\Thirdparty\Libs;%lib%
