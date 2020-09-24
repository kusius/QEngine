#include "EntityManager.h"
#include "Graphics/Model.h"

using namespace std;

GameObjects EntityManager::gameObjects;
unsigned int EntityManager::objectCount;

void EntityManager::Init()
{
    objectCount = 0;
}

void EntityManager::ImportModelFromFile(const char *path)
{
    Model m(path);
    //as datastructures
    //model -> meshes[]
    //mesh -> vertices[], indices[], textures[]
    GameObjects *gos = &(EntityManager::gameObjects);
    gos->numMeshes.push_back(m.meshes.size());
    for (unsigned int i = 0; i < m.meshes.size(); i++)
    {
        // Mesh data
        gos->numVertices.push_back(m.meshes[i].vertices.size());
        gos->numIndices.push_back(m.meshes[i].indices.size());
        gos->numTextures.push_back(m.meshes[i].textures.size());
        gos->vertices.insert(gos->vertices.end(),
                             m.meshes[i].vertices.begin(),
                             m.meshes[i].vertices.end());
        gos->indices.insert(gos->indices.end(),
                            m.meshes[i].indices.begin(),
                            m.meshes[i].indices.end());
        gos->textures.insert(gos->textures.end(),
                             m.meshes[i].textures.begin(),
                             m.meshes[i].textures.end());
        // World placement data
        gos->modelMatrices.push_back(glm::mat4(1.0));
    }
    objectCount++;
}

void EntityManager::TransformModel(unsigned int id, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    // TODO: Check if object with given id exists before indexing the array
    glm::mat4 transform = glm::mat4(1.0);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);
    EntityManager::gameObjects.modelMatrices[id] = transform;
}