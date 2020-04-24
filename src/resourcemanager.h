#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include <map>
#include <string>

#include "thirdparty/glad/glad.h"

#include "Graphics/Texture.h"
#include "Graphics/Shader.h"

/*
Singleton class ResourceManager. Hosts several functions to load
Textures and Shaders. Each of those texture and shaders is stored
for future retrieval by string handles. All functions and resources
are static and no public constructor is available.
*/
class ResourceManager
{
public:
	//Resource storage
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;
	//Recompile ALL shaders. TODO: Most likely a bad idea, done for testing now.
	static void RecompileShaders();
	//Load and generate a shader program given the source files
	static Shader LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
	//Retrieve stored shader
	static Shader *GetShader(std::string name);
	//Load and generate a texture from image file
	static Texture2D LoadTexture(const GLchar *file, GLboolean alpha, std::string name);
	//Retrieve stored texture
	static Texture2D GetTexture(std::string name);
	//Properly deallocate all resources
	static void Clear();

private:
	//private constructor . Resource manager object not available. Only members and functions
	ResourceManager() {}
	//Load and generate shader from file
	static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile);
	//Load and generate Texture from file
	static Texture2D loadTextureFromFile(const GLchar *file, GLboolean alpha);
};

#endif
