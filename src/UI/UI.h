#ifndef UI_H
#define UI_I

#include <UI/imgui.h>
#include <UI/imgui_impl_glfw.h>
#include <UI/imgui_impl_opengl3.h>
#include <UI/ImGuizmo.h>
#include <Metrics/CountDebugRegions.h>
#include <Managers/EntityManager.h>
#include <Thirdparty/entt/entt.hpp>
#include <Scene/Components.h>

#include <cstdint>
#include <vector>

namespace EditorUI
{

struct GameData
{
    std::vector<Render3DComponent> *gameObjects;
    entt::registry *registry;
    entt::entity closestRaycastEntity;
    int closestRaycastIndex;
    glm::mat4 *view, *projection;
    float lastFrameTime;
    bool bSnapTranslation;
    bool bRenderBoundingBoxes;
};

void SetupContext(GLFWwindow *window);
void Update(bool &uiWindow, bool &editorHasChanges, GameData &gameData);
void ShaderEditorOpenFile(const char *file);
void ShaderEditorSaveFile(const char *file, std::string &textToSave);
void EditTransform(const float *view, const float *projection, float *matrix);
void NewFrame();
void Render();
void Shutdown();
} // namespace EditorUI

#endif