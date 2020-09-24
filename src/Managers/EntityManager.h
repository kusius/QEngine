#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <Objects/GameObjects.h>

using namespace std;

class EntityManager
{
public:
    // Structure of arrays describing all gameobjects.
    static GameObjects gameObjects;
    static unsigned int objectCount;
    // Initialize the EntityManager class.
    static void Init();
    // Import a model from file, give a new ID and populate the gameObjects structure.
    static void ImportModelFromFile(const char *path);
    static void TransformModel(unsigned int id, glm::vec3 position = glm::vec3(0.0),
                               glm::vec3 rotation = glm::vec3(0.0),
                               glm::vec3 scale = glm::vec3(1.0));
    static void Translate(unsigned int id, glm::vec3 translation);
    static void Rotate(unsigned int id, glm::vec3 rotation);
    static void Scale(unsigned int id, glm::vec3 scale);

private:
    EntityManager() {}
};
#endif //ENTITY_MANAGER_H