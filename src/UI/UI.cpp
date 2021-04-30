#include <UI/TextEditor.h>
#include <UI/UI.h>
#include <Platform/Win64Platform.h>
#include <Thirdparty/glfw/glfw3.h>

#include <fstream>
#include <streambuf>
#include <string>
#include <iostream>

static TextEditor editor;
static auto lang = TextEditor::LanguageDefinition::GLSL();
static std::string currentFile;

static int selectedGameObjectIndex = -1;

void EditorUI::SetupContext(GLFWwindow *window)
{
    // Setup ImGui context
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    editor.SetLanguageDefinition(lang);
}

void EditorUI::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EditorUI::ShaderEditorOpenFile(const char *file)
{
    std::ifstream t(file);
    if (t.good())
    {
        std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
        editor.SetText(str);
    }
    t.close();
    currentFile = std::string(file);
}

void EditorUI::ShaderEditorSaveFile(const char *file, std::string &textToSave)
{
    std::ofstream t(file);
    if (t.good())
    {
        t << textToSave;
    }
    t.close();
}

void EditorUI::Update(bool &uiWindow, bool &hasChanges, GameData &gameData)
{
    auto cpos   = editor.GetCursorPosition();
    ImGuiIO &io = ImGui::GetIO();
    if (ImGui::BeginMainMenuBar())
    {

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
        ImGui::EndMainMenuBar();
    }
#ifdef QDEBUG
    if (uiWindow)
    {
        if (ImGui::Begin("Shader Editor", nullptr,
                         ImGuiWindowFlags_HorizontalScrollbar |
                             ImGuiWindowFlags_MenuBar))
        {
            ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

            if (io.KeyCtrl && io.KeysDown[GLFW_KEY_S])
            {
                std::string textToSave = editor.GetText();
                EditorUI::ShaderEditorSaveFile(currentFile.c_str(), textToSave);
                hasChanges = true;
            }

            if (ImGui::BeginMenuBar())
            {
                bool ro = editor.IsReadOnly();
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Save") ||
                        (io.KeyCtrl && io.KeysDown[GLFW_KEY_S]))
                    {
                        std::string textToSave = editor.GetText();
                        EditorUI::ShaderEditorSaveFile(currentFile.c_str(),
                                                       textToSave);
                        hasChanges = true;
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {

                    if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                        editor.SetReadOnly(ro);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr,
                                        !ro && editor.CanUndo()))
                        editor.Undo();
                    if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr,
                                        !ro && editor.CanRedo()))
                        editor.Redo();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr,
                                        editor.HasSelection()))
                        editor.Copy();
                    if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr,
                                        !ro && editor.HasSelection()))
                        editor.Cut();
                    if (ImGui::MenuItem("Delete", "Del", nullptr,
                                        !ro && editor.HasSelection()))
                        editor.Delete();
                    if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr,
                                        !ro && ImGui::GetClipboardText() !=
                                                   nullptr))
                        editor.Paste();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Select all", nullptr, nullptr))
                        editor.SetSelection(
                            TextEditor::Coordinates(),
                            TextEditor::Coordinates(editor.GetTotalLines(), 0));

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::MenuItem("Dark palette"))
                        editor.SetPalette(TextEditor::GetDarkPalette());
                    if (ImGui::MenuItem("Light palette"))
                        editor.SetPalette(TextEditor::GetLightPalette());
                    if (ImGui::MenuItem("Retro blue palette"))
                        editor.SetPalette(TextEditor::GetRetroBluePalette());
                    ImGui::EndMenu();
                }

                if (ImGui::Button("Close"))
                    uiWindow = false;

                ImGui::EndMenuBar();
                ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s",
                            cpos.mLine + 1, cpos.mColumn + 1,
                            editor.GetTotalLines(),
                            editor.IsOverwrite() ? "Ovr" : "Ins",
                            editor.CanUndo() ? "*" : " ",
                            editor.GetLanguageDefinition().mName.c_str(),
                            currentFile.c_str());
                editor.Render("ShaderEditor");
            }
        }
        ImGui::End(); // Shader Editor

        if (ImGui::Begin("Debug counters", nullptr, 0))
        {
            ImGui::SetWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
            for (unsigned int i = 0; i < NumRegions; i++)
            {
                ImGui::LabelText(DebugRegionStrings[i], "%.3f ms",
                                 TicksToMilliseconds(AvgCycles[i]));
            }
        }
        ImGui::End(); // Debug Counters

        if (ImGui::Begin("Entity Browser", nullptr, 0))
        {
            const int previousSelection = selectedGameObjectIndex;
            bool hasListSelection       = false;
            for (int i = 0; i < gameData.gameObjects->size(); i++)
            {
                string name =
                    string("(" + to_string(gameData.gameObjects->at(i).id) +
                           ") " + gameData.gameObjects->at(i).name);
                // See if there is a selection from the selectable list
                // if same item is selected act as toggle.
                if (ImGui::Selectable(name.c_str(),
                                      selectedGameObjectIndex == i))
                {
                    hasListSelection = true;
                    if (previousSelection != i)
                    {
                        selectedGameObjectIndex = i;
                        if (previousSelection >= 0)
                            EntityManager::UnsetFlags(
                                gameData.gameObjects->at(previousSelection),
                                FLAG_SELECTED);
                        EntityManager::SetFlags(gameData.gameObjects->at(i),
                                                FLAG_SELECTED);
                    }
                    else if (previousSelection == i && previousSelection >= 0)
                    {
                        selectedGameObjectIndex = -1;
                        EntityManager::UnsetFlags(
                            gameData.gameObjects->at(previousSelection),
                            FLAG_SELECTED);
                    }
                }
            }

            // Only process raycast selection if we aren't interacting with
            // Editor UI
            if (!io.WantCaptureMouse)
            {
                if (!gameData.gameObjects->empty() &&
                    gameData.closestRaycastIndex >= 0)
                {
                    if (previousSelection >= 0)
                        EntityManager::UnsetFlags(
                            gameData.gameObjects->at(previousSelection),
                            FLAG_SELECTED);
                    EntityManager::SetFlags(
                        gameData.gameObjects->at(gameData.closestRaycastIndex),
                        FLAG_SELECTED);
                    selectedGameObjectIndex = gameData.closestRaycastIndex;
                }
            }

            // Make the closestRaycastIndex as processed (invalidate)
            gameData.closestRaycastIndex = -1;
        }
        ImGui::End(); // EntityBrowser

        if (!gameData.gameObjects->empty() && selectedGameObjectIndex >= 0)
        {
            GameObject g = gameData.gameObjects->at(selectedGameObjectIndex);
            glm::mat4 *matrix =
                &(EntityManager::gameObjects
                      .modelMatrices[g.modelIndex][g.instanceIndex]);

            ImGuizmo::BeginFrame();
            ImGui::Begin("Editor");
            if (ImGui::RadioButton("Draw AABBs", gameData.bRenderBoundingBoxes))
                gameData.bRenderBoundingBoxes = !gameData.bRenderBoundingBoxes;

            ImGuizmo::SetID(0);
            EditTransform(glm::value_ptr(*gameData.view),
                          glm::value_ptr(*gameData.projection),
                          (float *)matrix);
            ImGui::End(); // Transform Guizmo
        }
    }
#endif
    ImGui::Render();
}

void EditorUI::EditTransform(const float *view, const float *projection,
                             float *matrix)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    if (ImGui::IsKeyPressed(GLFW_KEY_Z))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(GLFW_KEY_X))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(GLFW_KEY_C))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate",
                           mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate",
                           mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation,
                                          matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation, 3);
    ImGui::InputFloat3("Rt", matrixRotation, 3);
    ImGui::InputFloat3("Sc", matrixScale, 3);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation,
                                            matrixScale, matrix);

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    static bool useSnap(false);
    if (ImGui::IsKeyPressed(GLFW_KEY_MINUS))
        useSnap = !useSnap;
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    glm::vec3 snap(1.0f);
    switch (mCurrentGizmoOperation)
    {
    case ImGuizmo::TRANSLATE:
        // snap = config.mSnapTranslation;
        ImGui::InputFloat3("Snap", &snap.x);
        break;
    case ImGuizmo::ROTATE:
        // snap = config.mSnapRotation;
        ImGui::InputFloat("Angle Snap", &snap.x);
        break;
    case ImGuizmo::SCALE:
        // snap = config.mSnapScale;
        ImGui::InputFloat("Scale Snap", &snap.x);
        break;
    }
    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(view, projection, mCurrentGizmoOperation,
                         mCurrentGizmoMode, matrix, NULL,
                         useSnap ? &snap.x : NULL);
}

void EditorUI::Render()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorUI::Shutdown()
{
    editor.Delete();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}