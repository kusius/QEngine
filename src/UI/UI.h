#ifndef UI_H
#define UI_I

#include "UI/imgui.h"
#include "UI/imgui_impl_glfw.h"
#include "UI/imgui_impl_opengl3.h"

namespace UI
{
    struct UIInfo
    {
        float frameDelay;
    };

    void SetupContext(GLFWwindow *window);
    void UpdateUI(bool &uiWindow, bool &editorHasChanges, UIInfo *info);
    void ShaderEditorOpenFile(const char *file);
    void ShaderEditorSaveFile(const char *file, std::string &textToSave);
    void NewFrame();
    void Render();
    void Shutdown();
} // namespace UI

#endif