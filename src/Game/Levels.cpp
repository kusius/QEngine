#include "Levels.h"

#include <Scene/Components.h>
#include <Managers/EntityManager.h>

void Game::MakeLevel(entt::registry &registry)
{
    for (auto i = 0u; i < 100u; ++i)
    {
        const auto entity = registry.create();
        registry.emplace<TransformComponent>(entity, glm::vec3(0.5f * i),
                                             glm::vec3(0.0f), glm::vec3(0.0f),
                                             glm::vec3(1.0f, 3.0f, 1.0f));
        registry.emplace<ShapeComponent>(entity, Shape::CUBE);
        registry.emplace<RenderableComponent>(entity,
                                              glm::vec3((1.0f * i / 100)));
    }
}

void Game::MakeLevel(Renderer *renderer)
{
    Lights *lights                 = &(EntityManager::lights);
    lights->nPointLights           = 4; // this must be <= MAX_POINT_LIGHTS
    lights->pointLightPositions[0] = glm::vec3(1.7f, 0.2f, 2.0f);
    lights->pointLightPositions[1] = glm::vec3(2.3f, -3.3f, -4.0f);
    lights->pointLightPositions[2] = glm::vec3(-4.0f, 2.0f, -6.0f);
    lights->pointLightPositions[3] = glm::vec3(0.0f, 0.0f, -3.0f);

    EntityManager::Reset();
    for (auto i = 0u; i < 100u; ++i)
    {
        GameObject gameObject = EntityManager::ImportModelFromFile(
            "Assets/models/table/scene.gltf", "table");
        // EntityManager::TransformModel(gameObject, glm::vec3(0.5f * i),
        //                               glm::vec3(0.0f), glm::vec3(0.0f));
        EntityManager::TransformModel(
            gameObject, glm::vec3(0.0f, -2.0f * i, 2.5f * i),
            glm::vec3(-90.0f, 0.0f, 90.0f), glm::vec3(0.1f));

        gameObject = EntityManager::ImportModelFromFile(
            "Assets/Terrain/cobblestone_floor/cobblestone_floor.gltf", "tile");

        EntityManager::TransformModel(
            gameObject, glm::vec3(1.0f, -2.0f * i, 2.0f * i),
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f));
    }
    renderer->SetupShaderData();
}

const float rotateSpeed = 10.0f;
void Game::UpdateLevel(const float time)
{
    Lights *lights = &(EntityManager::lights);
    lights->pointLightPositions[1].y =
        6.0f * cos(rotateSpeed * glm::radians(time));
    lights->pointLightPositions[1].z =
        6.0f * sin(rotateSpeed * glm::radians(time));
    lights->pointLightPositions[2].x =
        6.0f * cos(rotateSpeed * glm::radians(time));
    lights->pointLightPositions[2].z =
        6.0f * sin(rotateSpeed * glm::radians(time));
}