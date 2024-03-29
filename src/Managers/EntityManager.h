#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <Objects/GameObjects.h>
#include <map>
#include <string>
#include <vector>

class EntityManager
{
  public:
    // Structure of arrays describing all gameobjects.
    static GameObjects gameObjects;
    // Structure holding the lights of a scene
    static Lights lights;

    // string: the path of the loaded model
    // unsigned int: the model index into the gameObjects SOA
    static std::map<string, unsigned int> loadedModels;

    static std::vector<glm::vec3> GetAABBVertices(const BoundingBox &bbox);

    // Initialize the EntityManager class.
    static void Reset();

    /**
     * @brief Import a model from path, give a new ID and store in gameObjects
     * @param path The full or relative path to the model resource file(gltf).
     * @return GameObject struct containing the instance and model ID as well as
     * the unique ID for that gameobject
     */
    static Render3DComponent ImportModelFromFile(const char *path,
                                                 const char *name = "unnamed");
    /**
     * @brief Take a texture directory and import specified amount of tiles
     * (quads) as terrain. By default placed in non-overlapping way around world
     * 0,0
     * @param texturesPath The folder containing the texture images.
     *
     * @param numInstances How many instances should be created
     * @param name A friendly name for this tile set
     */
    static std::vector<Render3DComponent>
    ImportTerrainQuadSet(const char *texturesPath, unsigned int numInstances,
                         const char *name = "unnamed_terrain");

    /**
     * @brief Transform one of the models stored in EntityManager::gameObjects
     * SOA
     *
     * @param id The unique id given to the object on import ( @ref
     * EntityManager::ImportModelFromFile )
     * @param move the amount of units to translate in x, y, z
     * @param rotation the amount of degrees to rotate around x, y, z axes
     * @param scale the amount to scale as a percentage of the original scale
     * in x, y, z
     */
    static void TransformModel(Render3DComponent &go, glm::vec3 move,
                               glm::vec3 rotation = glm::vec3(0.0),
                               glm::vec3 scale    = glm::vec3(1.0));

    static void EntityManager::TransformTerrainTile(Render3DComponent go,
                                                    glm::vec3 move,
                                                    glm::vec3 rotation,
                                                    glm::vec3 scale);
    // Translate a GameObject
    static void Translate(unsigned int id, glm::vec3 translation);
    // Rotate a GameObject
    static void Rotate(unsigned int id, glm::vec3 rotation);
    // Scale a GameObject
    static void Scale(unsigned int id, glm::vec3 scale);
    // Set a gameobject's flags
    static void SetFlags(Render3DComponent go, uint16_t flags);
    // Unset a gameobject's flags
    static void UnsetFlags(Render3DComponent go, uint16_t flags);
    // Get a gameobject's flags
    static uint16_t GetFlags(Render3DComponent go, uint16_t flags);
    // Check if a gameobject has the given flags
    static bool HasFlags(const Render3DComponent &go, const uint16_t &flags);
    static bool HasFlags(const uint32_t &modelIndex,
                         const uint32_t &instanceIndex, const uint16_t &flags);
    // Get a gameobject's bounding box in world coordinates
    static BoundingBox GetAABBWorld(const Render3DComponent &g);
    static BoundingBox GetAABBWorld(const uint32_t &modelIndex,
                                    const uint32_t instanceIndex);

  private:
    EntityManager(){};
    // The total objects stored in the gameObjects array
    static unsigned int nextInstanceID;
};
#endif // ENTITY_MANAGER_H