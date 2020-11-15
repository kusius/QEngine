#include "EntityManager.h"
#include <Graphics/Model.h>

using namespace std;

GameObjects EntityManager::gameObjects;
unsigned int EntityManager::nextInstanceID;
std::vector<BoundingBox> defaultBoundingBoxes;
std::map<string, unsigned int> EntityManager::loadedModels;

/**
 * @brief Perform a transform of an Axis-Aligned bounding box, keeping it AA.
 *
 * @param bbox the original bounding box of the model. Don't put already
 * transformed bounding boxes in this function
 * @param transform the matrix that holds the transform (translation, rotation
 * and scaling)
 */
static BoundingBox transformBoundingBox(const BoundingBox &bbox,
                                        const glm::mat4 &transform);

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
  thisGameObject.rayCastSelected = false;
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
    std::vector<BoundingBox> aabbs;
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
      aabbs.push_back(m.meshes[i].boundingBox);
    }

    // World placement data
    gos->modelMatrices.push_back({glm::mat4(1.0f)});
    gos->positions.push_back({glm::vec3(0.0f)});
    gos->angles.push_back({glm::vec3(0.0f)});
    gos->scales.push_back({glm::vec3(1.0f)});

    defaultBoundingBoxes.push_back(m.AABB);
    // State data
    gos->flags.push_back({0x0000});
  }

  nextInstanceID++;
  return thisGameObject;
}

glm::mat4 aabbModelMatrix(const BoundingBox &bbox)
{
  // 1. calculate size (scale)
  glm::vec3 scale =
      glm::vec3(bbox.mMax.x - bbox.mMin.x, bbox.mMax.y - bbox.mMin.y,
                bbox.mMax.z - bbox.mMin.z);
  // 2. calculate center (position)
  glm::vec3 center = glm::vec3((bbox.mMin.x + bbox.mMax.x) / 2,
                               (bbox.mMin.y + bbox.mMax.y) / 2,
                               (bbox.mMin.z + bbox.mMax.z) / 2);
  // perform translations
  glm::mat4 transform = glm::mat4(1.0f);
  // translate
  transform = glm::translate(transform, center);
  // scale
  transform = glm::scale(transform, glm::vec3(scale));
  return transform;
}

// NOTE: If we need, we can store this world AABB, per object for quicker access
BoundingBox EntityManager::GetAABBWorld(const GameObject &g)
{
  BoundingBox box = defaultBoundingBoxes[g.modelIndex];
  glm::mat4 model =
      EntityManager::gameObjects.modelMatrices[g.modelIndex][g.instanceIndex];

  glm::vec4 worldMin = model * glm::vec4(box.mMin, 1.0f); // point
  glm::vec4 worldMax = model * glm::vec4(box.mMax, 1.0f); // point

  BoundingBox result = {glm::vec3(worldMin.x, worldMin.y, worldMin.z),
                        glm::vec3(worldMax.x, worldMax.y, worldMax.z)};
  return result;
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

std::vector<glm::vec3> EntityManager::GetAABBVertices(const BoundingBox &bbox)
{
  std::vector<glm::vec3> vertices(8, glm::vec3(0.0f));
  vertices[0] = bbox.mMin;
  vertices[1] = glm::vec3(bbox.mMin.x, bbox.mMin.y, bbox.mMax.z);
  vertices[2] = glm::vec3(bbox.mMin.x, bbox.mMax.y, bbox.mMin.z);
  vertices[3] = glm::vec3(bbox.mMax.x, bbox.mMin.y, bbox.mMin.z);
  vertices[4] = glm::vec3(bbox.mMin.x, bbox.mMax.y, bbox.mMax.z);
  vertices[5] = glm::vec3(bbox.mMax.x, bbox.mMin.y, bbox.mMax.z);
  vertices[6] = glm::vec3(bbox.mMax.x, bbox.mMax.y, bbox.mMin.z);
  vertices[7] = bbox.mMax;

  return vertices;
}

static BoundingBox transformBoundingBox(const BoundingBox &bbox,
                                        const glm::mat4 &transform)
{
  std::vector<glm::vec3> vertices = EntityManager::GetAABBVertices(bbox);

  glm::vec3 newMin =
      glm::vec3(1.0f, 1.0f, 1.0f) * std::numeric_limits<float>::max();
  glm::vec3 newMax =
      glm::vec3(1.0f, 1.0f, 1.0f) * std::numeric_limits<float>::min();
  for (int i = 0; i < 8; ++i)
  {
    glm::vec4 transformed = transform * glm::vec4(vertices[i], 1.0f);
    for (int j = 0; j < 3; ++j)
    {
      newMin[j] = glm::min(newMin[j], transformed[j]);
      newMax[j] = glm::max(newMax[j], transformed[j]);
    }
  }
  BoundingBox newbbox = {newMin, newMax};
  return newbbox;
}

void EntityManager::SetFlags(GameObject go, uint16_t flags)
{
  EntityManager::gameObjects.flags[go.modelIndex][go.instanceIndex] |= flags;
}

void EntityManager::UnsetFlags(GameObject go, uint16_t flags)
{
  EntityManager::gameObjects.flags[go.modelIndex][go.instanceIndex] &= ~flags;
}

uint16_t EntityManager::GetFlags(GameObject go, uint16_t flags)
{
  return EntityManager::gameObjects.flags[go.modelIndex][go.instanceIndex] &
         flags;
}