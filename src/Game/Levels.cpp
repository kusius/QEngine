#include "Levels.h"

#include <Scene/Components.h>
#include <Managers/EntityManager.h>
#include <Thirdparty/glfw/glfw3.h>

#include <iostream>
Game::Game()
{
    tileModelPaths[NORMAL] =
        "Assets/terrain/cobblestone_floor/cobblestone_floor.gltf";
}

void Game::placeLevelTiles(const std::vector<std::vector<TILE_TYPE>> &level,
                           entt::registry &registry)
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    float offset       = 0.5f;
    string tilePath    = "";
    for (auto row : level)
    {
        for (TILE_TYPE tile : row)
        {
            string tilePath = this->tileModelPaths[tile];
            switch (tile)
            {
            case NORMAL:
            {
                Render3DComponent tile =
                    EntityManager::ImportModelFromFile(tilePath.c_str());

                EntityManager::TransformModel(tile, position,
                                              glm::vec3(0.0f, 0.0f, 0.0f),
                                              glm::vec3(0.001f));
                entt::entity entity = registry.create();
                registry.emplace<Render3DComponent>(entity, tile);
            }
            case NONE:
            case GOAL:
                position = position + glm::vec3(offset, 0.0f, 0.0f);
            default:
                break;
            }
        }
        position = glm::vec3(0.0f, 0.0f, position.z + offset);
    }
}

void Game::MakeLevel(Renderer *renderer, entt::registry &registry)
{
    Lights *lights                 = &(EntityManager::lights);
    lights->nPointLights           = 4; // this must be <= MAX_POINT_LIGHTS
    lights->pointLightPositions[0] = glm::vec3(1.7f, 0.2f, 2.0f);
    lights->pointLightPositions[1] = glm::vec3(2.3f, -3.3f, -4.0f);
    lights->pointLightPositions[2] = glm::vec3(-4.0f, 2.0f, -6.0f);
    lights->pointLightPositions[3] = glm::vec3(0.0f, 0.0f, -3.0f);

    EntityManager::Reset();
    placeLevelTiles(Game::level1, registry);

    // Place player
    Render3DComponent player = EntityManager::ImportModelFromFile(
        "Assets/primitives/3d/cube/scene.gltf", "cube");
    EntityManager::TransformModel(player, glm::vec3(0.0f, 0.0f, 0.0f),
                                  glm::vec3(-90.0f, 0.0f, 90.0f),
                                  glm::vec3(0.1f, 0.1f, 0.2f));

    entt::entity entity = registry.create();
    registry.emplace<Render3DComponent>(entity, player);

    renderer->SetupShaderData();
    ShaderStaticData(renderer);
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

void Game::ShaderStaticData(Renderer *renderer)
{
    const Lights lights = EntityManager::lights;
    Shader *shader      = renderer->shader;
    // GL_TEXTURE1 = diffuse, GL_TEXTURE2 = specular, GL_TEXTURE3 = emission,
    // GL_TEXTURE4 = normal
    shader->Use();
    shader->SetInteger("material.texture_diffuse1", 0);
    shader->SetInteger("material.texture_specular1", 1);
    shader->SetInteger("material.texture_emission1", 2);
    // set the shininess
    shader->SetFloat("material.shininess", 10.0f);

    //******** DIRECTIONAL LIGHT ************
    shader->SetVector3f("dirLight.direction", 1.0f, 1.0f, 1.0f);
    shader->SetVector3f("dirLight.ambient", glm::vec3(0.0f)); // a dark ambient
    shader->SetVector3f("dirLight.diffuse",
                        glm::vec3(0.5f)); // darken the light a bit
    shader->SetVector3f("dirLight.specular", glm::vec3(1.0f)); // full white

    for (unsigned int i = 0; i < lights.nPointLights; i++)
    {
        std::string number = std::to_string(i);

        shader->SetVector3f(("pointLights[" + number + "].position").c_str(),
                            lights.pointLightPositions[i]);
        shader->SetVector3f(("pointLights[" + number + "].ambient").c_str(),
                            glm::vec3(0.1f));
        shader->SetVector3f(("pointLights[" + number + "].diffuse").c_str(),
                            glm::vec3(0.5f));
        shader->SetVector3f(("pointLights[" + number + "].specular").c_str(),
                            glm::vec3(1.0f));
        shader->SetFloat(("pointLights[" + number + "].constant").c_str(),
                         1.0f);
        shader->SetFloat(("pointLights[" + number + "].linear").c_str(), 0.09f);
        shader->SetFloat(("pointLights[" + number + "].quadratic").c_str(),
                         0.032f);
    }

    //***************** SPOTLIGHT ****************
    shader->SetVector3f("spotLight.ambient", glm::vec3(0.1f)); // a dark ambient
    shader->SetVector3f("spotLight.diffuse",
                        glm::vec3(1.0f)); // darken the light a bit
    shader->SetVector3f("spotLight.specular", glm::vec3(1.0f)); // full white
    shader->SetFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
    shader->SetFloat("spotLight.outercutoff", glm::cos(glm::radians(17.5f)));
}
