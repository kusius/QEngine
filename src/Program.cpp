#include <Platform/Win64Platform.h>
#include <Metrics/CountDebugRegions.h>
#include <Graphics/Renderer.h>
#include <Scene/Components.h>
#include <Managers/EntityManager.h>
#include <Graphics/Camera.h>
#include <Managers/ResourceManager.h>
#include <Thirdparty/assimp/version.h>
#include <Thirdparty/glad/glad.h>
#include <Thirdparty/glfw/glfw3.h>
#include <Thirdparty/entt/entt.hpp>
#include <Game/Levels.h>
#include <UI/UI.h>

#include <limits>
#include <iostream>
#include <string>

enum WindowMode
{
    WINDOWED_FULLSCREEN,
    WINDOWED,
};

// Function forward declarations
void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode);
void MouseCallback(GLFWwindow *window, double xpos, double ypos);
void MouseButtonCallBack(GLFWwindow *window, int button, int action, int mods);
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
void ProcessInput(GLFWwindow *window, float deltaTime, Renderer *renderer);

// World and game data
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
std::vector<Render3DComponent> gameObjects = {};
glm::mat4 projection                       = glm::mat4(1);
EditorUI::GameData gameDataForEditor;
entt::registry registry;
Game game;

// Video properties
static int targetRefreshRate;
static double targetFrameTime;
static int screenWidth              = 800;
static int screenHeight             = 600;
static int windowPos[2]             = {0, 0};
static WindowMode currentWindowMode = WindowMode::WINDOWED_FULLSCREEN;

// mouse controls
bool firstMouse         = true;
bool mouseAffectsCamera = true;
bool disableMouse       = false;
double lastX            = (float)screenWidth / 2;
double lastY            = (float)screenHeight / 2;

// keyboard states (https://www.glfw.org/docs/latest/group__keys.html)
// TODO(George): Consider an InputManager class
bool key_states[512] = {false};

// light position
glm::vec3 lightPos(1.2, 1.0f, 2.0f); // in global space coordinates

bool uiWindow         = false;
bool uiWindowFocused  = true;
bool editorHasChanges = false;

void ChangeWindowMode(const WindowMode windowMode, GLFWwindow *window)
{
    GLFWmonitor *monitor    = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    switch (windowMode)
    {
    case WINDOWED_FULLSCREEN:
        // save window position for restoring later
        glfwGetWindowPos(window, &windowPos[0], &windowPos[1]);
        glfwGetWindowSize(window, &screenWidth, &screenHeight);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height,
                             mode->refreshRate);
        currentWindowMode = windowMode;
        break;
    case WINDOWED:
        glfwSetWindowMonitor(window, nullptr, windowPos[0], windowPos[1],
                             screenWidth, screenHeight, mode->refreshRate);
        currentWindowMode = windowMode;
        break;
    default:
        std::cout << "[ERROR] Unknown window mode: " << currentWindowMode
                  << std::endl;
    }
}

GLFWwindow *QCreateWindow(WindowMode currentWindowMode)
{
    GLFWwindow *window;
    if (!glfwInit())
        return NULL;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *monitor    = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    screenWidth             = mode->width;
    screenHeight            = mode->height;

    switch (currentWindowMode)
    {
    case WindowMode::WINDOWED_FULLSCREEN:
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window =
            glfwCreateWindow(mode->width, mode->height, "Q", monitor, nullptr);
        break;
    case WindowMode::WINDOWED:
    default:
        glfwWindowHint(GLFW_DECORATED, true);
        glfwWindowHint(GLFW_MAXIMIZED, true);
        glfwWindowHint(GLFW_RESIZABLE, true);
        window =
            glfwCreateWindow(screenWidth, screenHeight, "Q", nullptr, nullptr);
        break;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return NULL;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallBack);

    return window;
}

void SetupOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
}

void PrintLibVersions()
{
    std::cout << "Assimp: " << aiGetVersionMajor() << "." << aiGetVersionMinor()
              << "." << aiGetVersionPatch() << std::endl;
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;
    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    std::cout << "GLFW: " << major << "." << minor << "rev" << rev << std::endl;
}

entt::entity SelectClosestObject(double mousex, double mousey, int width,
                                 int height, entt::registry &registry)
{
    float minDistance = std::numeric_limits<float>::max();
    float distance;
    entt::entity closest = entt::null;
    auto eView           = registry.view<const Render3DComponent>();
    // Ray starting from camera in world coordinates
    Ray rayWorld = camera.mouseposToRayWorld(mousex, mousey, width, height);

    for (auto entity : eView)
    {
        auto &gameObject  = eView.get<const Render3DComponent>(entity);
        int modelIndex    = gameObject.modelIndex;
        int instanceIndex = gameObject.instanceIndex;
        // Bounding box in world coordinates
        BoundingBox box = EntityManager::GetAABBWorld(gameObject);

        if (camera.rayAABBIntersection(box.mMin, box.mMax, rayWorld, nullptr))
        {
            float d1 = glm::distance(rayWorld.o, box.mMin);
            float d2 = glm::distance(rayWorld.o, box.mMax);
            distance = glm::min(d1, d2);
            if (distance < minDistance)
            {
                closest     = entity;
                minDistance = distance;
            }
        }
    }
    return closest;
}

int main(int argc, char **argv)
{
    currentWindowMode  = WindowMode::WINDOWED;
    GLFWwindow *window = QCreateWindow(currentWindowMode);
    if (!window)
        return -1;

    SetupOpenGL();
#ifdef QDEBUG
    PrintLibVersions();
#endif

    //*****************
    // VIEW AND PROJECTION MATRICES for orthographic camera
    //******************
    glm::mat4 view = glm::mat4(1);
    view           = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection     = glm::perspective(glm::radians(camera.Zoom),
                                  (float)screenWidth / (float)screenHeight,
                                  0.1f, 100.0f);

    /*****************
    RESOURCE LOADING
    ******************/

    // Make our shaders (read, compile, link)
    Shader *shader = ResourceManager::LoadShader(
        "Assets/Shaders/gameobject.vert", "Assets/Shaders/gameobject.frag",
        nullptr, "shader");
    Shader *lightShader = ResourceManager::LoadShader(
        "Assets/Shaders/lampshader.vert", "Assets/Shaders/lampshader.frag",
        nullptr, "lampshader");
    Shader *highlightShader = ResourceManager::LoadShader(
        "Assets/Shaders/gameobject.vert", "Assets/Shaders/diffusecolor.frag",
        nullptr, "highlight");

    Renderer *renderer;
    renderer = new Renderer(*shader, *highlightShader, *lightShader);

    // EntityManager::TransformModel(table3,
    // glm::vec3(0.0f, -2.0f, 6.5f),
    //                            glm::vec3(-90.0f, 0.0f, 90.0f),
    // glm::vec3(0.1f));

    // renderer prepares data based on current gameObjects SOA

    double thisTime       = 0.0;
    double deltaTime      = 0.0;
    double startFrameTime = 0.0;
    double endFrameTime   = 0.0;

    game.MakeLevel(renderer, registry);

    gameDataForEditor                      = {&gameObjects, 0};
    gameDataForEditor.registry             = &registry;
    gameDataForEditor.view                 = &view;
    gameDataForEditor.projection           = &projection;
    gameDataForEditor.bRenderBoundingBoxes = false;

    EditorUI::SetupContext(window);
#ifdef QDEBUG
    EditorUI::ShaderEditorOpenFile("Assets/Shaders/gameobject.frag");
#endif

    InitPlatform();

#ifdef QDEBUG
    InitDebug();
#endif
    auto renderables = registry.view<const Render3DComponent>();
    while (!glfwWindowShouldClose(window))
    {
        BEGIN_DEBUG_REGION(MainLoop);
        startFrameTime = glfwGetTime();

        BEGIN_DEBUG_REGION(ProcessInput);
        ProcessInput(window, (float)deltaTime, renderer);
        END_DEBUG_REGION(ProcessInput);

        BEGIN_DEBUG_REGION(UpdateWorldObjects);
        projection = glm::perspective(glm::radians(camera.Zoom),
                                      (float)screenWidth / (float)screenHeight,
                                      0.1f, 100.0f);
        camera.SetProjectionMatrix(projection);
        view = camera.GetViewMatrix();

        game.UpdateLevel((float)startFrameTime);
        END_DEBUG_REGION(UpdateWorldObjects);

        BEGIN_DEBUG_REGION(UpdateDebugUI);
        EditorUI::NewFrame();
        EditorUI::Update(uiWindow, editorHasChanges, gameDataForEditor);
        if (editorHasChanges)
        {
            ResourceManager::RecompileShaders();
            game.ShaderStaticData(renderer);
            editorHasChanges = false;
        }
        uiWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
        END_DEBUG_REGION(UpdateDebugUI);

        BEGIN_DEBUG_REGION(Draw);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);

        renderer->BeginFrame(camera);
        renderer->DrawGameObjects();
        renderer->DrawPointLights(glm::vec3(0.2f), 0.0f,
                                  glm::vec3(1.0f, 0.0f, 0.0f));

        // auto eView =
        //     registry.view<const TransformComponent, const ShapeComponent,
        //                   const RenderableComponent>();
        // for (auto [entity, transform, shape, renderable] : eView.each())
        // {
        //     lightRenderer->DrawShape(transform, shape, renderable);
        // }

#ifdef QDEBUG
        if (gameDataForEditor.bRenderBoundingBoxes)
        {
            // for (int i = 0; i < gameObjects.size(); ++i)
            // {
            //     Render3DComponent g = gameObjects.at(i);
            //     renderer->DrawBoundingBox(EntityManager::GetAABBWorld(g),
            //                               GL_TRUE);
            // }
            renderer->DrawBoundingBoxes();
        }
#endif
        EditorUI::Render();

        glfwSwapBuffers(window);
        END_DEBUG_REGION(Draw);

        BEGIN_DEBUG_REGION(PollInput);
        glfwPollEvents();
        END_DEBUG_REGION(PollInput);

        thisTime  = glfwGetTime();
        deltaTime = thisTime - startFrameTime;
        END_DEBUG_REGION(MainLoop);
    }

    EditorUI::Shutdown();
    ResourceManager::Clear();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void MouseButtonCallBack(GLFWwindow *window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        gameDataForEditor.closestRaycastEntity = SelectClosestObject(
            xpos, ypos, screenWidth, screenHeight, registry);
    }
}

// NOTE(George): Callback way is not used right now, we process input at the
// beginning of a frame
void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

inline bool ProcessKeyTap(int key_code, GLFWwindow *window)
{
    bool ret = false;
    if (key_code < 0)
        return ret;
    if (glfwGetKey(window, key_code) == GLFW_PRESS && !key_states[key_code])
    {
        key_states[key_code] = true;
        ret                  = true;
    }
    else if (glfwGetKey(window, key_code) != GLFW_PRESS)
    {
        key_states[key_code] = false;
        ret                  = false;
    }
    return ret;
}

void ProcessInput(GLFWwindow *window, float deltaTime, Renderer *renderer)
{
    if (ProcessKeyTap(GLFW_KEY_ESCAPE, window))
    {
        uiWindow = !uiWindow;
        if (uiWindow)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseAffectsCamera = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseAffectsCamera = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS &&
        ProcessKeyTap(GLFW_KEY_ENTER, window))
    {
        // TODO: Properly change between fullscreen and windowed mode
    }

    if (!uiWindow || !uiWindowFocused)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);
        if (ProcessKeyTap(GLFW_KEY_R, window))
        {
            ResourceManager::RecompileShaders();
            game.ShaderStaticData(renderer);
        }
        if (ProcessKeyTap(GLFW_KEY_P, window))
        {
            GLint polygonMode;
            glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
            if (polygonMode == GL_LINE)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        if (ProcessKeyTap(GLFW_KEY_SPACE, window))
        {
        }
    }
    if (ProcessKeyTap(GLFW_KEY_F1, window))
    {
        GLint status;
        status = glfwGetInputMode(window, GLFW_CURSOR);
        if (status == GLFW_CURSOR_DISABLED)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseAffectsCamera = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseAffectsCamera = true;
        }
    }
}

void MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX      = xpos;
        lastY      = ypos;
        firstMouse = false;
    }

    double dx = xpos - lastX;
    double dy = lastY - ypos;
    lastX     = xpos;
    lastY     = ypos;

    if (mouseAffectsCamera)
    {
        dx *= camera.MouseSensitivity;
        dy *= camera.MouseSensitivity;

        camera.ProcessMouseMovement(dx, dy, true);
    }
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (mouseAffectsCamera)
        camera.ProcessMouseScroll(yoffset);
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    if (width > 0 && height > 0)
    {
        glViewport(0, 0, width, height);
        screenHeight = height;
        screenWidth  = width;
        projection   = glm::mat4(1);
        projection   = glm::perspective(glm::radians(camera.Zoom),
                                      (float)screenWidth / (float)screenHeight,
                                      0.1f, 100.0f);
        camera.SetProjectionMatrix(projection);
    }
}