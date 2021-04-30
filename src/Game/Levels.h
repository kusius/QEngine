#pragma once
#include <Thirdparty/entt/entt.hpp>
#include <Graphics/Renderer.h>

namespace Game
{
void MakeLevel(entt::registry &registry);
void MakeLevel(Renderer *renderer);
void UpdateLevel(const float time);
} // namespace Game