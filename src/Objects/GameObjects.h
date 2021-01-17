#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#define MAX_POINT_LIGHTS 4

#include <Thirdparty/glm/glm.hpp>
#include <Thirdparty/assimp/aabb.h>
#include <Graphics/Mesh.h>

#include <vector>

#define MAX_GAME_OBJECTS 512

#define FLAG_NONE (0x0000)
#define FLAG_SELECTED (0x0001)

// Exposes data to the game
struct GameObject
{
  std::string name; // friendly name given on creation
  std::string
      path; // the path of the descriptor file for the object (.gltf etc.)
  unsigned int id, modelIndex, instanceIndex;
  bool rayCastSelected;
};

struct Lights
{
  unsigned int nPointLights = MAX_POINT_LIGHTS;
  glm::vec3 pointLightPositions[MAX_POINT_LIGHTS];
};

struct Terrain
{
  glm::vec3 position;
};

// SOA used to store GameObjects
struct GameObjects
{
  std::vector<uint32_t> numInstances;
  std::vector<uint32_t> numMeshes; // size: number unique models

  // how many textures, vertices and indices does each mesh have
  std::vector<uint32_t> numTextures;
  std::vector<uint32_t> numVertices;
  std::vector<uint32_t> numIndices;
  // all gameObject data into single arrays
  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  std::vector<Texture> textures;

  // modelMatrices stores numInstances matrices for that gameObject
  std::vector<std::vector<glm::mat4>> modelMatrices;
  std::vector<std::vector<glm::vec3>> positions;
  std::vector<std::vector<glm::vec3>> scales;
  std::vector<std::vector<glm::vec3>> angles;
  std::vector<std::vector<uint16_t>> flags;
};

struct GameObjectsInstanced
{
  std::vector<glm::mat4> modelMatrices;
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> scales;
  std::vector<glm::vec3> angles;

  std::vector<Texture> textures; // one set of texture maps for all instances
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  std::vector<uint16_t> flags;
  uint32_t
      numTextures; // number of texture types(maps) stored in textures array
};

#endif // GAME_OBJECTS_H