#ifndef UI_H
#define UI_I

#include "UI/imgui.h"
#include "UI/imgui_impl_glfw.h"
#include "UI/imgui_impl_opengl3.h"
#include <Metrics/CountDebugRegions.h>
#include <Managers/EntityManager.h>

#include <cstdint>
#include <vector>

namespace EditorUI
{

struct GameData
{
  std::vector<GameObject> *gameObjects;
  float lastFrameTime;
};

void SetupContext(GLFWwindow *window);
void Update(bool &uiWindow, bool &editorHasChanges, GameData &gameData);
void ShaderEditorOpenFile(const char *file);
void ShaderEditorSaveFile(const char *file, std::string &textToSave);
void NewFrame();
void Render();
void Shutdown();
} // namespace EditorUI

#endif