#ifndef UI_H
#define UI_I

#include "UI/imgui.h"
#include "UI/imgui_impl_glfw.h"
#include "UI/imgui_impl_opengl3.h"
#include <Metrics/CountDebugRegions.h>

#include <cstdint>

namespace UI
{

struct UIInfo
{
  float lastFrameTime;
};

void SetupContext(GLFWwindow *window);
void Update(bool &uiWindow, bool &editorHasChanges);
void ShaderEditorOpenFile(const char *file);
void ShaderEditorSaveFile(const char *file, std::string &textToSave);
void NewFrame();
void Render();
void Shutdown();
} // namespace UI

#endif