#include <iostream>
#include <string>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "ResourceManager.h"
#include "Graphics/SpriteRenderer.h"
#include "Objects/Entity.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow * window, float deltaTime);
const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;


//setup camera
//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


//setup mouse controls
bool firstMouse = GL_TRUE;
float lastX = (float) SCREEN_WIDTH / 2;
float lastY = (float) SCREEN_HEIGHT / 2 ; 



//light position
glm::vec3 lightPos(1.2, 1.0f, 2.0f); //in global space coordinates
float rotateSpeed = 10.0f;


int main(int argc, char** argv)
{
    
    /*****************
    OPENGL-GLFW- CONFIGURATION
    ******************/
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asset Import", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }
    
    //glfw set 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /*****************
      VIEW AND PROJECTION MATRICES for orthographic camera
     ******************/
    glm::mat4 view = glm::mat4(1);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::mat4(1);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / float(SCREEN_HEIGHT), 0.1f, 100.0f);
    
    
    /*****************
    RESOURCE LOADING
    ******************/
    ResourceManager::LoadTexture("container2.png", GL_FALSE, "container");
    ResourceManager::LoadTexture("container2_specular.png", GL_FALSE, "container_specular");
    ResourceManager::LoadTexture("matrix.jpg", GL_FALSE, "container_emission");
    Texture2D diffusemap = ResourceManager::GetTexture("container");
    Texture2D specularmap = ResourceManager::GetTexture("container_specular");
    Texture2D emissionmap = ResourceManager::GetTexture("container_emission");
    
    
    //Make our shaders (read, compile, link)
    ResourceManager::LoadShader("../AssetImport/Shaders/shader.vert", "../AssetImport/Shaders/lightingshader.frag", nullptr, "shader");
    ResourceManager::LoadShader("../AssetImport/Shaders/lampshader.vert", "../AssetImport/Shaders/lampshader.frag", nullptr, "lampshader");
    
    
    
    //3D set info to shader
    Shader shader = ResourceManager::GetShader("shader");
    Shader lightShader = ResourceManager::GetShader("lampshader");
    
    //Set positioning uniforms
    //shader.SetMatrix4("model", model, GL_TRUE); //YOU NEED TO ACTIVATE SHADER
    shader.Use();
    shader.SetMatrix4("view", view);   //YOU NEED TO ACTIVATE SHADER
    shader.SetMatrix4("projection", projection);
    
    
    /************************
  **********MATERIAL*******
  **************************/
    
    //GL_TEXTURE1 = diffuse, GL_TEXTURE2 = specular, GL_TEXTURE3 = emission
    shader.SetInteger("material.texture_diffuse1",0);
    shader.SetInteger("material.texture_specular1",1);
    shader.SetInteger("material.texture_emission1",2);
    //set the shininess
    shader.SetFloat("material.shininess", 200.0f);
    
    /************************
  **********LIGHTS*********
  **************************/
    
    
    //******** DIRECTIONAL LIGHT ************
    //shader.SetVector3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.SetVector3f("dirLight.direction", 1.0f, 1.0f, 1.0f);
    shader.SetVector3f("dirLight.ambient", glm::vec3(0.0f)); //a dark ambient
    shader.SetVector3f("dirLight.diffuse", glm::vec3(0.5f)); //darken the light a bit 
    shader.SetVector3f("dirLight.specular", glm::vec3(1.0f)); //full white
    
    
    //************* POINT LIGHT(S) **************
    glm::vec3 pointLightPositions[] = {
        glm::vec3(1.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
    
    for (unsigned int i = 0; i < 4; i++)
    {
        std::string number = std::to_string(i);
        
        
        shader.SetVector3f( ("pointLights[" + number + "].position").c_str(), pointLightPositions[i]);
        shader.SetVector3f(("pointLights[" + number + "].ambient").c_str(), glm::vec3(0.1f));
        shader.SetVector3f(("pointLights[" + number + "].diffuse").c_str(), glm::vec3(0.5f));
        shader.SetVector3f(("pointLights[" + number + "].specular").c_str(), glm::vec3(1.0f));
        shader.SetFloat(("pointLights[" + number + "].constant").c_str(), 1.0f);
        shader.SetFloat(("pointLights[" + number + "].linear").c_str(), 0.09f);
        shader.SetFloat(("pointLights[" + number + "].quadratic").c_str(), 0.032f);
    }
    
    
    //***************** SPOTLIGHT ****************
    //direction and position set in the render loop
    shader.SetVector3f("spotLight.ambient", glm::vec3(0.1f)); //a dark ambient
    shader.SetVector3f("spotLight.diffuse", glm::vec3(1.0f)); //darken the light a bit 
    shader.SetVector3f("spotLight.specular", glm::vec3(1.0f)); //full white
    shader.SetFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
    shader.SetFloat("spotLight.outercutoff", glm::cos(glm::radians(17.5f)));
    
    
    
    
    //************** RENDER THE LIGHT OBJECTS *****************
    lightShader.Use();
    lightShader.SetMatrix4("view", view);   //YOU NEED TO ACTIVATE SHADER
    lightShader.SetMatrix4("projection", projection);
    
    
    /************************
  **********MODEL*********
  **************************/
    Entity ftm("../AssetImport/Models/naruto/naruto.obj");
    ftm.SetView(view);
    ftm.SetProjection(projection);
    ftm.Scale(glm::vec3(-0.8f, -0.8f, -0.8f));
    //ftm.Rotate(glm::vec3(0.0f, 0.785f, 0.0f), glm::vec3(0.0f,0.0f,0.0f)
    //);
    
    //Model naruto("Models/lu/scene.gltf");
    //Model nanosuit("Models/bunny/untitled.obj");
    //Model moxxi("Models/moxxi/Moxxi.obj");
    //Model puss("Models/puss/puss.obj");
    
    //************SETUP RENDERER OBJECTS ***********
    //Init all render data
    Renderer  *renderer, *lightRenderer;
    renderer = new Renderer(shader);
    lightRenderer = new Renderer(lightShader);
    
    //wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    float lightDistance = glm::distance(glm::vec3(0.0f), lightPos);
    
    /*****************
    MAIN GAME LOOP
    ******************/
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        
        processInput(window, deltaTime);
        //Render into backbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        
        
        
        //to move the light
        //lightPos.x = lightDistance * cos(rotateSpeed * glm::radians(glfwGetTime()));
        //lightPos.z = lightDistance *  sin(rotateSpeed * glm::radians(glfwGetTime()));
        
        //Set out spotlight to follow camera 
        shader.Use();
        shader.SetVector3f("spotLight.position", camera.Position);
        shader.SetVector3f("spotLight.direction", camera.Front);
        
        //set view position to use for directional light
        shader.SetVector3f("viewPos", camera.Position);
        
        
        shader.Use();
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);
        
        //DIRECTIONAL LIGHT VERTEX SHADER UNIFORMS
        lightShader.Use();
        lightShader.SetMatrix4("view", view);
        lightShader.SetMatrix4("projection", projection);
        
        //**** RENDER ALL OBJECTS FOR THIS FRAME ****
        
        
        //Render our point lights in the scene
        for (unsigned int i = 0; i < 4; i++)
        {
            lightRenderer->DrawSprite(pointLightPositions[i], glm::vec3(0.2f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
        
        //Render our cube
        renderer->DrawSprite(diffusemap, specularmap, emissionmap, glm::vec3(0.0f), glm::vec2(1.0f), rotateSpeed * glm::radians(glfwGetTime()));
        //renderer->DrawSprite(glm::vec3(0.0f), glm::vec3(1.0f), 0.0f, glm::vec3(204.0f, 0.0f, 102.0f));
        
        
        ftm.SetView(view);
        ftm.SetProjection(projection);
        ftm.Draw(shader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        //TODO: Check for changed shader programs and reload them, recompile them
        
    }
    
    
    ResourceManager::Clear(); //it's static class so doesnt have destructor clear necessary!
    glfwTerminate();
    return 0;
    
}



void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


void processInput(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float cameraSpeed = 4.5 * deltaTime;// * deltaTime;
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
    
}


void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
    
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = GL_FALSE;
    }
    float dx = xpos - lastX;
    float dy = lastY -ypos ;
    lastX = xpos;
    lastY = ypos;
    
    dx *= camera.MouseSensitivity;
    dy *= camera.MouseSensitivity;
    
    camera.ProcessMouseMovement(dx, dy, GL_TRUE);
    
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
