#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#define MAX_POINT_LIGHTS 4

#include "Thirdparty/glm/glm.hpp"
#include "Graphics/Mesh.h"
#include <vector>

struct Lights
{
  unsigned int nPointLights = MAX_POINT_LIGHTS;
  glm::vec3 pointLightPositions[MAX_POINT_LIGHTS];
};

struct GameObjects
{
  std::vector<unsigned int> numInstances;
  std::vector<unsigned int> numMeshes; // size: number unique models

  // how many textures, vertices and indices does each mesh have
  std::vector<unsigned int> numTextures;
  std::vector<unsigned int> numVertices;
  std::vector<unsigned int> numIndices;
  // all gameObject data into single arrays
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  // modelMatricec stores numInstances matrices for that gameObject
  std::vector<std::vector<glm::mat4>> modelMatrices;
  std::vector<std::vector<glm::vec3>> positions;
  std::vector<std::vector<glm::vec3>> scales;
  std::vector<std::vector<glm::vec3>> angles;
};

#endif // GAME_OBJECTS_H