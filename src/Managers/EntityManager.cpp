#include "EntityManager.h"
#include "Graphics/Model.h"

using namespace std;

GameObjects EntityManager::gameObjects;
unsigned int EntityManager::nextInstanceID;
std::map<string, unsigned int> EntityManager::loadedModels;

void EntityManager::Init()
{
  nextInstanceID = 0;
  loadedModels.clear();
}

GameObject EntityManager::ImportModelFromFile(const char *path,
                                              const char *name)
{
  GameObjects *gos = &(EntityManager::gameObjects);
  unsigned int thisInstanceID = nextInstanceID;
  GameObject thisGameObject = {};
  thisGameObject.name = std::string(name);
  thisGameObject.path = std::string(path);

  auto it = loadedModels.find(std::string(path));
  if (it != loadedModels.end())
  {
    thisGameObject.id = thisInstanceID;
    thisGameObject.modelIndex = it->second;
    thisGameObject.instanceIndex = gos->modelMatrices[it->second].size();

    gos->modelMatrices[it->second].push_back(glm::mat4(1.0f));
    gos->positions[it->second].push_back(glm::vec3(0.0f));
    gos->angles[it->second].push_back(glm::vec3(0.0f));
    gos->scales[it->second].push_back(glm::vec3(1.0f));
    gos->flags[it->second].push_back(0x0000);
  }
  else
  {
    Model m(path);
    // as datastructures
    // model -> meshes[]
    // mesh -> vertices[], indices[], textures[]
    // model index into gameObjects SOA
    thisGameObject.id = thisInstanceID;
    thisGameObject.instanceIndex = 0;
    thisGameObject.modelIndex = gos->numMeshes.size();
    loadedModels[std::string(path)] = gos->numMeshes.size();
    gos->numMeshes.push_back(m.meshes.size());
    for (unsigned int i = 0; i < m.meshes.size(); i++)
    {
      // Mesh data
      gos->numVertices.push_back(m.meshes[i].vertices.size());
      gos->numIndices.push_back(m.meshes[i].indices.size());
      gos->numTextures.push_back(m.meshes[i].textures.size());

      gos->vertices.insert(gos->vertices.end(), m.meshes[i].vertices.begin(),
                           m.meshes[i].vertices.end());

      gos->indices.insert(gos->indices.end(), m.meshes[i].indices.begin(),
                          m.meshes[i].indices.end());

      gos->textures.insert(gos->textures.end(), m.meshes[i].textures.begin(),
                           m.meshes[i].textures.end());
    }

    // World placement data
    gos->modelMatrices.push_back({glm::mat4(1.0f)});
    gos->positions.push_back({glm::vec3(0.0f)});
    gos->angles.push_back({glm::vec3(0.0f)});
    gos->scales.push_back({glm::vec3(1.0f)});
    // State data
    gos->flags.push_back({0x0000});
  }

  nextInstanceID++;
  return thisGameObject;
}

void EntityManager::TransformModel(GameObject go, glm::vec3 move,
                                   glm::vec3 rotation, glm::vec3 scale)
{
  if (go.id < MAX_GAME_OBJECTS && go.id < nextInstanceID)
  {
    EntityManager::gameObjects.positions[go.modelIndex][go.instanceIndex] +=
        move;
    EntityManager::gameObjects.angles[go.modelIndex][go.instanceIndex] =
        rotation;
    EntityManager::gameObjects.scales[go.modelIndex][go.instanceIndex] = scale;
    glm::mat4 transform = glm::mat4(1.0);
    transform = glm::translate(transform, move);
    transform = glm::rotate(transform, glm::radians(rotation.x),
                            glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y),
                            glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);
    EntityManager::gameObjects.modelMatrices[go.modelIndex][go.instanceIndex] =
        transform;
  }
}

void EntityManager::SetFlags(GameObject go, uint16_t flags)
{
  EntityManager::gameObjects.flags[go.modelIndex][go.instanceIndex] |= flags;
}