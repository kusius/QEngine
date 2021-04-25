#include "ResourceManager.h"

#include <Graphics/stb_image.h>
#include <Thirdparty/assimp/Importer.hpp>
#include <Thirdparty/assimp/scene.h>
#include <Thirdparty/assimp/postprocess.h>

#include <iostream>
#include <sstream>
#include <fstream>

// instantiate static variables
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Texture> ResourceManager::AITextures;

Shader *ResourceManager::LoadShader(const GLchar *vShaderFile,
                                    const GLchar *fShaderFile,
                                    const GLchar *gShaderFile, std::string name)
{
  Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
  return &Shaders[name];
}

Shader *ResourceManager::GetShader(std::string name) { return &Shaders[name]; }

Texture2D ResourceManager::LoadTexture(const GLchar *file, GLboolean alpha,
                                       std::string name)
{
  Textures[name] = ResourceManager::loadTextureFromFile(file, alpha);
  return Textures[name];
}

std::vector<Texture> ResourceManager::LoadMaterialGLTF(std::string path)
{
  std::string directory = path.substr(0, path.find_last_of('/'));
  std::vector<Texture> textures;
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
    return textures;
  }

  aiNode *mRootNode = scene->mRootNode;
  aiMesh *mMesh = scene->mMeshes[mRootNode->mMeshes[0]];
  if (mMesh->mMaterialIndex >= 0)
  {
    aiMaterial *mMaterial = scene->mMaterials[mMesh->mMaterialIndex];
    std::vector<Texture> mTextures;
    mTextures = loadMaterialTextures(mMaterial, aiTextureType_DIFFUSE,
                                     "texture_diffuse", directory);
    textures.insert(textures.end(), mTextures.begin(), mTextures.end());

    mTextures = loadMaterialTextures(mMaterial, aiTextureType_NORMALS,
                                     "texture_normal", directory);
    textures.insert(textures.end(), mTextures.begin(), mTextures.end());
  }
  return textures;
}

std::vector<Texture>
ResourceManager::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                      const char *typeName,
                                      std::string directory)
{
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    auto it = AITextures.find(std::string(str.C_Str()));
    if (it != AITextures.end())
    {
      textures.push_back(it->second);
      skip = true;
    }
    if (!skip)
    {
      Texture texture;
      texture.id = loadTextureFromFile(str.C_Str(), directory);
      texture.type = typeName;
      texture.path =
          str.C_Str(); // NOTE: This is not path it's texture's file name
      textures.push_back(texture);
      AITextures[std::string(str.C_Str())] = texture;
    }
  }

  return textures;
}

Texture2D ResourceManager::GetTexture(std::string name)
{
  return Textures[name];
}

void ResourceManager::Clear()
{
  // Delete all shaders
  for (auto it : Shaders)
    glDeleteProgram(it.second.ID);
  // Delete all textures
  for (auto it : Textures)
    glDeleteTextures(1, &it.second.ID);
}

void ResourceManager::recompileShaders()
{
  for (auto it : Shaders)
    ResourceManager::LoadShader(it.second.vShaderFile.c_str(),
                                it.second.fShaderFile.c_str(), nullptr,
                                it.first);
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile,
                                           const GLchar *fShaderFile,
                                           const GLchar *gShaderFile)
{
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;

  try
  {
    // Open the files
    std::ifstream vertexShaderFile(vShaderFile);
    std::ifstream fragmentShaderFile(fShaderFile);
    std::stringstream vShaderStream, fShaderStream;
    // Read the file's buffer contents into streams
    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();
    // close file handles
    vertexShaderFile.close();
    fragmentShaderFile.close();
    // Stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    if (gShaderFile != nullptr)
    {
      std::ifstream geometryShaderFile(gShaderFile);
      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  }

  catch (std::exception e)
  {
    std::cout << "[*] ERROR::SHADER: Failed to read shader files" << std::endl;
  }

  const GLchar *vShaderCode = vertexCode.c_str();
  const GLchar *fShaderCode = fragmentCode.c_str();
  const GLchar *gShaderCode = geometryCode.c_str();

  Shader shader;
  shader.Compile(vShaderCode, fShaderCode,
                 gShaderFile != nullptr ? gShaderCode : nullptr);
  shader.vShaderFile = vShaderFile;
  shader.fShaderFile = fShaderFile;
  return shader;
}

unsigned int ResourceManager::loadTextureFromFile(const char *file,
                                                  std::string &directory)
{
  unsigned int id;
  // generate the texture
  glGenTextures(1, &id);

  int width, height, bpp;
  std::string path = directory + '/' + file;
  std::cout << "LOAD::TEXTURE::INFO: Loading texture " << path << std::endl;
  unsigned char *image =
      stbi_load((directory + '/' + file).c_str(), &width, &height, &bpp, 0);

  if (!image)
  {
    std::cout << "LOAD_TEXTURE::FAILED: " << stbi_failure_reason() << " "
              << path << std::endl;
    stbi_image_free(image);
    return id;
  }

  GLenum format;
  if (bpp == 1)
    format = GL_RED;
  else if (bpp == 3)
    format = GL_RGB;
  else if (bpp == 4)
    format = GL_RGBA;

  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // unbind the texture
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);
  return id;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file,
                                               GLboolean alpha)
{
  Texture2D texture;
  if (alpha)
  {
    texture.Internal_Format = GL_RGBA;
    texture.Image_Format = GL_RGBA;
    int channels = 4;
  }
  // Load image
  int width, height, bpp;
  unsigned char *image = stbi_load(file, &width, &height, &bpp, alpha ? 4 : 3);
  if (!image)
  {
    std::cout << "LOAD_TEXTURE::FAILED" << std::endl;
  }
  // gen the texture
  texture.Generate(width, height, image);
  // free image data
  stbi_image_free(image);
  return texture;
}