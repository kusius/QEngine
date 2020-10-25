#include "EntityManager.h"
#include "Graphics/Model.h"

using namespace std;

GameObjects EntityManager::gameObjects;
unsigned int EntityManager::nextInstanceID;
InstanceMap EntityManager::instanceMap;

void EntityManager::Init()
{
  nextInstanceID = 0;
  instanceMap.clear();
}

unsigned int EntityManager::ImportModelFromFile(const char *path)
{
  GameObjects *gos = &(EntityManager::gameObjects);
  unsigned int thisInstanceID = nextInstanceID;
  // If we get asked to load the same model (same path) we just
  // duplicate the data
  // NOTE: right now we are just saving the id of the first occurence of the
  // modelt without tracking which models inside gameObjects are the same.
  // In the future we would want to draw identical objects with the likes of
  // glDraw*Instanced
  StringKey sk = {string(path)};
  auto it = instanceMap.find(sk);
  if (it != instanceMap.end())
  {
    // for now, inserting a duplicate model, means tracking
    // different modelMatrix, positions, scales and angles
    // for the same vertices, indices and textures data
    cout << "Existing model: " << it->first.modelPath
         << " with instance ID: " << it->first.instanceID << endl;
    cout << "Inserting new instance of model: " << string(path)
         << " with instanceID" << thisInstanceID << endl;

    EntityKey k = {string(path), thisInstanceID};
    instanceMap[k] = gos->modelMatrices.size();

    gos->modelMatrices.push_back(glm::mat4(1.0f));
    gos->positions.push_back(glm::vec3(0.0f));
    gos->angles.push_back(glm::vec3(0.0f));
    gos->scales.push_back(glm::vec3(1.0f));
  }
  else
  {
    Model m(path);
    // as datastructures
    // model -> meshes[]
    // mesh -> vertices[], indices[], textures[]
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

    // instance tracking
    EntityKey k = {string(path), thisInstanceID};
    instanceMap[k] = gos->modelMatrices.size();

    // World placement data
    gos->modelMatrices.push_back(glm::mat4(1.0f));
    gos->positions.push_back(glm::vec3(0.0f));
    gos->angles.push_back(glm::vec3(0.0f));
    gos->scales.push_back(glm::vec3(1.0f));
  }

  nextInstanceID++;
  return thisInstanceID;
}

void EntityManager::TransformModel(unsigned int id, glm::vec3 move,
                                   glm::vec3 rotation, glm::vec3 scale)
{
  IntKey ik = {id};
  auto instance = instanceMap.find(ik);
  if (instance != instanceMap.end())
  {
    unsigned int index = instance->second;

    EntityManager::gameObjects.positions[index] += move;
    EntityManager::gameObjects.angles[index] = rotation;
    EntityManager::gameObjects.scales[index] = scale;
    glm::mat4 transform = glm::mat4(1.0);
    transform = glm::translate(transform, move);
    transform = glm::rotate(transform, glm::radians(rotation.x),
                            glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y),
                            glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);
    EntityManager::gameObjects.modelMatrices[index] = transform;
  }
}

bool operator<(const EntityKey &ek, const StringKey &sk)
{
  return strcmp(sk.modelPath.c_str(), ek.modelPath.c_str());
}
bool operator<(const StringKey &sk, const EntityKey &ek) { return ek < sk; }

bool operator<(const EntityKey &ek, const IntKey &ik)
{
  return ek.instanceID < ik.instanceID;
}
bool operator<(const IntKey &ik, const EntityKey &ek) { return ek < ik; }

bool operator<(const EntityKey &left, const EntityKey &right)
{
  return left.instanceID < right.instanceID &&
         strcmp(left.modelPath.c_str(), right.modelPath.c_str());
}