#include <iostream>
#include <string>

#include <Thirdparty/glad/glad.h>
#include <Thirdparty/glfw/glfw3.h>
#include <Thirdparty/assimp/version.h>

#include <Camera.h>
#include <ResourceManager.h>
#include <Graphics/SpriteRenderer.h>
#include <Objects/Entity.h>
#include <UI/UI.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void ProcessInput(GLFWwindow *window, float deltaTime, Shader *hader, Shader *lightShader);
void ShaderStaticData(Shader *shader, Shader *lightShader);
GLuint SCREEN_WIDTH = 800;
GLuint SCREEN_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

//mouse controls
bool firstMouse = GL_TRUE;
bool mouseAffectsCamera = GL_TRUE;
bool disableMouse = GL_FALSE;
float lastX = (float)SCREEN_WIDTH / 2;
float lastY = (float)SCREEN_HEIGHT / 2;

//keyboard states (https://www.glfw.org/docs/latest/group__keys.html)
//TODO(George): Consider an InputManager class
bool key_states[512] = {GL_FALSE};

//light position
glm::vec3 lightPos(1.2, 1.0f, 2.0f); //in global space coordinates

//************* POINT LIGHT(S) **************
glm::vec3 pointLightPositions[] = {
    glm::vec3(1.7f, 0.2f, 2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -6.0f),
    glm::vec3(0.0f, 0.0f, -3.0f)};

//************ENTITIES***********//
//TODO(George): Consider merging these into ResourceManager
Entity *ftm, *ftm1;
vector<Entity *> entities;

float rotateSpeed = 10.0f;

bool uiWindow = GL_FALSE;
bool uiWindowFocused = GL_FALSE;
bool editorHasChanges = GL_FALSE;
UI::UIInfo info = {};

GLFWwindow *CreateWindow()
{
    GLFWwindow *window;
    if (!glfwInit())
        return NULL;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    /*
    //Windowed Fullscreen
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    window = glfwCreateWindow(mode->width, mode->height, "Q", glfwGetPrimaryMonitor(), nullptr);
*/
    SCREEN_WIDTH = mode->width;
    SCREEN_HEIGHT = mode->height;
    window = glfwCreateWindow(mode->width, mode->height, "Q", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    //glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return NULL;
    }

    //glfw cursor grab mode
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
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
}

int main(int argc, char **argv)
{

    GLFWwindow *window = CreateWindow();
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
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / float(SCREEN_HEIGHT), 0.1f, 100.0f);

    /*****************
    RESOURCE LOADING
    ******************/
    //ResourceManager::LoadTexture("Assets/container2.png", GL_FALSE, "container");
    //ResourceManager::LoadTexture("Assets/container2_specular.png", GL_FALSE, "container_specular");
    //ResourceManager::LoadTexture("Assets/matrix.jpg", GL_FALSE, "container_emission");
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
    ftm = new Entity("Assets/models/table/scene.gltf");
    ftm1 = new Entity("Assets/models/old_sofa/scene.gltf");
    ftm->Move(glm::vec3(0.0f, -2.0f, 2.5f));
    ftm->Scale(glm::vec3(-0.9f, -0.9f, -0.9f));
    ftm->Rotate(glm::vec3(-90.0f, 0.0f, 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ftm1->Move(glm::vec3(0.0f, -2.0f, -1.5f));

    entities.push_back(ftm);
    entities.push_back(ftm1);

    //************SETUP RENDERER OBJECTS ***********
    Renderer *renderer, *lightRenderer;
    renderer = new Renderer(*shader, *highlightShader);
    lightRenderer = new Renderer(*lightShader);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    UI::SetupContext(window);
    UI::ShaderEditorOpenFile("Assets/Shaders/lightingshader.frag");
    //*****************
    //MAIN LOOP
    //******************

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        ProcessInput(window, deltaTime, shader, lightShader);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        pointLightPositions[2].x = 6.0 * cos(rotateSpeed * glm::radians(glfwGetTime()));
        pointLightPositions[2].z = 6.0 * sin(rotateSpeed * glm::radians(glfwGetTime()));

        pointLightPositions[1].y = 6.0 * cos(rotateSpeed * glm::radians(glfwGetTime()));
        pointLightPositions[1].z = 6.0 * sin(rotateSpeed * glm::radians(glfwGetTime()));

        shader->Use();
        shader->SetVector3f("spotLight.position", camera.Position);
        shader->SetVector3f("spotLight.direction", camera.Front);
        shader->SetVector3f("pointLights[2].position", pointLightPositions[2]);
        shader->SetVector3f("pointLights[1].position", pointLightPositions[1]);
        shader->SetVector3f("viewPos", camera.Position);
        shader->SetMatrix4("view", view);
        shader->SetMatrix4("projection", projection);

        highlightShader->Use();
        highlightShader->SetMatrix4("view", view);
        highlightShader->SetMatrix4("projection", projection);

        lightShader->Use();
        lightShader->SetMatrix4("view", view);
        lightShader->SetMatrix4("projection", projection);

        UI::NewFrame();
        UI::UpdateUI(uiWindow, editorHasChanges, &info);
        if (editorHasChanges)
        {
            ResourceManager::RecompileShaders();
            ShaderStaticData(shader, lightShader);
            editorHasChanges = GL_FALSE;
        }
        uiWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        lightRenderer->DrawPointLights(pointLightPositions, 4, glm::vec3(0.2));
        renderer->DrawEntities(entities);
        UI::Render();

        glfwSwapBuffers(window);
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

    for (unsigned int i = 0; i < 4; i++)
    {
        std::string number = std::to_string(i);

        shader->SetVector3f(("pointLights[" + number + "].position").c_str(), pointLightPositions[i]);
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
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

inline bool ProcessKeyTap(int key_code, GLFWwindow *window)
{
    bool ret = GL_FALSE;
    if (key_code < 0)
        return ret;
    if (glfwGetKey(window, key_code) == GLFW_PRESS &&
        !key_states[key_code])
    {
        key_states[key_code] = GL_TRUE;
        ret = GL_TRUE;
    }
    else if (glfwGetKey(window, key_code) != GLFW_PRESS)
    {
        key_states[key_code] = GL_FALSE;
        ret = GL_FALSE;
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
            mouseAffectsCamera = GL_FALSE;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseAffectsCamera = GL_TRUE;
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
            GLint polygonMode[2];
            glGetIntegerv(GL_POLYGON_MODE, polygonMode);
            if (polygonMode[1] == GL_LINE)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        if (ProcessKeyTap(GLFW_KEY_SPACE, window))
        {
            ftm->isSelected = !ftm->isSelected;
        }
    }
    if (ProcessKeyTap(GLFW_KEY_F1, window))
    {
        GLint status;
        status = glfwGetInputMode(window, GLFW_CURSOR);
        if (status == GLFW_CURSOR_DISABLED)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseAffectsCamera = GL_FALSE;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseAffectsCamera = GL_TRUE;
        }
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = GL_FALSE;
    }
    float dx = xpos - lastX;
    float dy = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (mouseAffectsCamera)
    {
        dx *= camera.MouseSensitivity;
        dy *= camera.MouseSensitivity;

        camera.ProcessMouseMovement(dx, dy, GL_TRUE);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (mouseAffectsCamera)
        camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
