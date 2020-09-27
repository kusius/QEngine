#ifdef _WIN32
#include <Platform/Win64Platform.h>
#endif

#include <iostream>
#include <string>

#include <Thirdparty/glad/glad.h>
#include <Thirdparty/glfw/glfw3.h>
#include <Thirdparty/assimp/version.h>

#include <Camera.h>
#include <ResourceManager.h>
#include <Managers/EntityManager.h>
#include <Graphics/SpriteRenderer.h>
#include <Objects/Entity.h>
#include <UI/UI.h>

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xpos, double ypos);
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
void ProcessInput(GLFWwindow *window, float deltaTime, Shader *hader, Shader *lightShader);
void ShaderStaticData(Shader *shader, Shader *lightShader);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

Lights lights = {};

//Video properties
static int targetRefreshRate;
static double targetFrameTime;
static unsigned int screenWidth = 800;
static unsigned int screenHeight = 600;

//mouse controls
bool firstMouse = true;
bool mouseAffectsCamera = true;
bool disableMouse = false;
float lastX = (float)screenWidth / 2;
float lastY = (float)screenHeight / 2;

//keyboard states (https://www.glfw.org/docs/latest/group__keys.html)
//TODO(George): Consider an InputManager class
bool key_states[512] = {false};

//light position
glm::vec3 lightPos(1.2, 1.0f, 2.0f); //in global space coordinates

//************ENTITIES***********//
//TODO(George): Consider merging these into ResourceManager
vector<Entity> entities;

bool uiWindow = false;
bool uiWindowFocused = true;
bool editorHasChanges = false;

GLFWwindow *QCreateWindow()
{
    GLFWwindow *window;
    if (!glfwInit())
        return NULL;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    screenWidth = mode->width;
    screenHeight = mode->height;
    targetRefreshRate = 2 * mode->refreshRate;
    targetFrameTime = 1.0 / targetRefreshRate;

    /*
    //Windowed Fullscreen
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    window = glfwCreateWindow(mode->width, mode->height, "Q", glfwGetPrimaryMonitor(), nullptr);
    */

    //maximized window
    //*
    glfwWindowHint(GLFW_DECORATED, true);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_RESIZABLE, true);
    window = glfwCreateWindow(mode->width, mode->height, "Q", nullptr, nullptr);
    //*/

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return NULL;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

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
    std::cout << "Assimp: " << aiGetVersionMajor() << "." << aiGetVersionMinor() << std::endl;
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    std::cout << "GLFW: " << major << "." << minor << "rev" << rev << std::endl;
}

int main(int argc, char **argv)
{
    GLFWwindow *window = QCreateWindow();
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
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::mat4(1);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

    /*****************
    RESOURCE LOADING
    ******************/
    lights.nPointLights = 4; // this must be <= MAX_POINT_LIGHTS
    lights.pointLightPositions[0] = glm::vec3(1.7f, 0.2f, 2.0f);
    lights.pointLightPositions[1] = glm::vec3(2.3f, -3.3f, -4.0f);
    lights.pointLightPositions[2] = glm::vec3(-4.0f, 2.0f, -6.0f);
    lights.pointLightPositions[3] = glm::vec3(0.0f, 0.0f, -3.0f);

    //ResourceManager::LoadTexture("Assets/container2.png", false, "container");
    //ResourceManager::LoadTexture("Assets/container2_specular.png", false, "container_specular");
    //ResourceManager::LoadTexture("Assets/matrix.jpg", false, "container_emission");
    //Texture2D diffusemap = ResourceManager::GetTexture("container");
    //Texture2D specularmap = ResourceManager::GetTexture("container_specular");
    //Texture2D emissionmap = ResourceManager::GetTexture("container_emission");

    //Make our shaders (read, compile, link)
    ResourceManager::LoadShader("Assets/Shaders/shader.vert", "Assets/Shaders/lightingshader.frag", nullptr, "shader");
    ResourceManager::LoadShader("Assets/Shaders/lampshader.vert", "Assets/Shaders/lampshader.frag", nullptr, "lampshader");
    ResourceManager::LoadShader("Assets/Shaders/shader.vert", "Assets/Shaders/diffusecolor.frag", nullptr, "highlight");
    Shader *shader = ResourceManager::GetShader("shader");
    Shader *lightShader = ResourceManager::GetShader("lampshader");
    Shader *highlightShader = ResourceManager::GetShader("highlight");

    ShaderStaticData(shader, lightShader);

    //Load models
    
    EntityManager::Init();
    EntityManager::ImportModelFromFile("Assets/models/table/scene.gltf");
    EntityManager::ImportModelFromFile("Assets/models/old_sofa/scene.gltf");
    //EntityManager::ImportModelFromFile("Assets/models/old_sofa/scene.gltf");
    EntityManager::TransformModel(0, glm::vec3(0.0f, -2.0f, 5.5f), glm::vec3(-90.0f, 0.0f, 90.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    //EntityManager::TransformModel(0, glm::vec3(0.0f, -2.0f, 5.5f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(-0.9f, -0.9f, -0.9f));
    
    

    Entity e = Entity("Assets/models/table/scene.gltf");
    e.Move(glm::vec3(0.0f, 0.0f, -1.5f));
    e.Scale(glm::vec3(-0.9f, -0.9f, -0.9f));
    entities.push_back(e);

    //************SETUP RENDERER OBJECTS ***********
    Renderer *renderer, *lightRenderer;
    renderer = new Renderer(*shader, *highlightShader);
    lightRenderer = new Renderer(*lightShader);

    renderer->SetupMeshes();
    /*
    Entity e("Assets/models/table/scene.gltf");
    e.Move(glm::vec3(0.0f, -2.0f, 2.5f));
    e.Scale(glm::vec3(-0.9f, -0.9f, -0.9f));
    e.Rotate(glm::vec3(-90.0f, 0.0f, 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    entities.push_back(e);
    */

    

    double thisTime = 0.0;
    double deltaTime = 0.0;
    double startFrameTime = 0.0;
    double endFrameTime = 0.0;
    float rotateSpeed = 10.0f;
    int64_t hdStartTime = 0;
    int64_t hdEndTime = 0;

    UI::SetupContext(window);
    UI::ShaderEditorOpenFile("Assets/Shaders/lightingshader.frag");
    //*****************
    //MAIN LOOP
    //******************
    while (!glfwWindowShouldClose(window))
    {
        startFrameTime = glfwGetTime();
        hdStartTime = Platform::GetWallClockTime();

        ProcessInput(window, (float)deltaTime, shader, lightShader);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        lights.pointLightPositions[1].y = 6.0 * cos(rotateSpeed * glm::radians(startFrameTime));
        lights.pointLightPositions[1].z = 6.0 * sin(rotateSpeed * glm::radians(startFrameTime));
        lights.pointLightPositions[2].x = 6.0 * cos(rotateSpeed * glm::radians(startFrameTime));
        lights.pointLightPositions[2].z = 6.0 * sin(rotateSpeed * glm::radians(startFrameTime));

        UI::NewFrame();
        UI::UpdateUI(uiWindow, editorHasChanges);
        if (editorHasChanges)
        {
            ResourceManager::RecompileShaders();
            ShaderStaticData(shader, lightShader);
            editorHasChanges = false;
        }
        uiWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        shader->Use();
        shader->SetVector3f("spotLight.position", camera.Position);
        shader->SetVector3f("spotLight.direction", camera.Front);
        shader->SetVector3f("pointLights[2].position", lights.pointLightPositions[2]);
        shader->SetVector3f("pointLights[1].position", lights.pointLightPositions[1]);
        shader->SetVector3f("viewPos", camera.Position);
        shader->SetMatrix4("view", view);
        shader->SetMatrix4("projection", projection);

        highlightShader->Use();
        highlightShader->SetMatrix4("view", view);
        highlightShader->SetMatrix4("projection", projection);

        renderer->DrawEntities(entities);
        renderer->DrawGameObjects();

        lightShader->Use();
        lightShader->SetMatrix4("view", view);
        lightShader->SetMatrix4("projection", projection);
        lightRenderer->DrawPointLights(lights.pointLightPositions, lights.nPointLights, glm::vec3(0.2));

        UI::Render();

        // NOTE(George): Fixed framerate:
        // we let glfw sync with monitor refresh rate.
        // If we want to implement a fixed framerate, we must switch to
        // swapping buffers by ourselves(?)
        glfwSwapBuffers(window);
        glfwPollEvents();

        thisTime = glfwGetTime();
        deltaTime = thisTime - startFrameTime;
        hdEndTime = Platform::GetWallClockTime();
        //std::cout << "Frame time: " << (float)Platform::DiffTimeMilliseconds(hdStartTime, hdEndTime) << std::endl;
    }

    UI::Shutdown();
    ResourceManager::Clear();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void ShaderStaticData(Shader *shader, Shader *lightShader)
{
    //GL_TEXTURE1 = diffuse, GL_TEXTURE2 = specular, GL_TEXTURE3 = emission, GL_TEXTURE4 = normal
    shader->Use();
    shader->SetInteger("material.texture_diffuse1", 0);
    shader->SetInteger("material.texture_specular1", 1);
    shader->SetInteger("material.texture_emission1", 2);
    //set the shininess
    shader->SetFloat("material.shininess", 10.0f);

    //******** DIRECTIONAL LIGHT ************
    shader->SetVector3f("dirLight.direction", 1.0f, 1.0f, 1.0f);
    shader->SetVector3f("dirLight.ambient", glm::vec3(0.0f));  //a dark ambient
    shader->SetVector3f("dirLight.diffuse", glm::vec3(0.5f));  //darken the light a bit
    shader->SetVector3f("dirLight.specular", glm::vec3(1.0f)); //full white

    for (unsigned int i = 0; i < lights.nPointLights; i++)
    {
        std::string number = std::to_string(i);

        shader->SetVector3f(("pointLights[" + number + "].position").c_str(), lights.pointLightPositions[i]);
        shader->SetVector3f(("pointLights[" + number + "].ambient").c_str(), glm::vec3(0.1f));
        shader->SetVector3f(("pointLights[" + number + "].diffuse").c_str(), glm::vec3(0.5f));
        shader->SetVector3f(("pointLights[" + number + "].specular").c_str(), glm::vec3(1.0f));
        shader->SetFloat(("pointLights[" + number + "].constant").c_str(), 1.0f);
        shader->SetFloat(("pointLights[" + number + "].linear").c_str(), 0.09f);
        shader->SetFloat(("pointLights[" + number + "].quadratic").c_str(), 0.032f);
    }

    //***************** SPOTLIGHT ****************
    shader->SetVector3f("spotLight.ambient", glm::vec3(0.1f));  //a dark ambient
    shader->SetVector3f("spotLight.diffuse", glm::vec3(1.0f));  //darken the light a bit
    shader->SetVector3f("spotLight.specular", glm::vec3(1.0f)); //full white
    shader->SetFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
    shader->SetFloat("spotLight.outercutoff", glm::cos(glm::radians(17.5f)));
}

//NOTE(George): Callback way is not used right now, we process input at the beginning of a frame
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

inline bool ProcessKeyTap(int key_code, GLFWwindow *window)
{
    bool ret = false;
    if (key_code < 0)
        return ret;
    if (glfwGetKey(window, key_code) == GLFW_PRESS &&
        !key_states[key_code])
    {
        key_states[key_code] = true;
        ret = true;
    }
    else if (glfwGetKey(window, key_code) != GLFW_PRESS)
    {
        key_states[key_code] = false;
        ret = false;
    }
    return ret;
}

void ProcessInput(GLFWwindow *window, float deltaTime, Shader *shader, Shader *lightShader)
{
    if (ProcessKeyTap(GLFW_KEY_ESCAPE, window))
    {
        //glfwSetWindowShouldClose(window, true);
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
            ShaderStaticData(shader, lightShader);
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
            entities[0].isSelected = !entities[0].isSelected;
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
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float dx = xpos - lastX;
    float dy = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

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
    glViewport(0, 0, width, height);
}
