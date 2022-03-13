#pragma once
#include <Thirdparty/entt/entt.hpp>
#include <Graphics/Renderer.h>
#include <Graphics/Camera.h>

#include <map>
enum TILE_TYPE
{
    NONE,
    NORMAL,
    LIGHT_PAD,
    STRONG_PAD,
    BRIDGE,
    GOAL,

    // Keep always last
    NUM_TILES
};

class Game
{
  public:
    Game();

    void MakeLevel(Renderer *renderer, entt::registry &registry);
    void UpdateLevel(const float time);
    void ShaderStaticData(Renderer *renderer);
    void ShutDown();
    void ProcessKeyboard(CameraMovement movement, float deltaTime,
                         const entt::registry &registry);
    entt::entity GetPlayer() const;
    void ProcessMouseMovement(const float &dx, const float &dy,
                              const Camera &camera,
                              const entt::registry &registry);

  private:
    void placeLevelTiles(const std::vector<std::vector<TILE_TYPE>> &level,
                         entt::registry &registry);
    const std::vector<std::vector<TILE_TYPE>> level1{
        {NORMAL, NORMAL, NORMAL, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
         NONE},

        {NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NONE, NONE, NONE,
         NONE},

        {NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL,
         NONE},

        {NONE, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL,
         NORMAL},

        {NONE, NONE, NONE, NONE, NONE, NORMAL, GOAL, NORMAL, NORMAL, NORMAL},

        {NONE, NONE, NONE, NONE, NONE, NONE, NORMAL, NORMAL, NORMAL, NONE}};

    std::array<string, NUM_TILES> tileModelPaths;
    entt::entity playerEntity;
};