#include <Windows.h>
#include <iostream>
#include <Thirdparty/glad/glad.h>
#include <Thirdparty/glfw/glfw3.h>

// NOTE(George): To implement in program
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

//Video properties
static int targetRefreshRate;
static double targetFrameTime;
static unsigned int screenWidth = 800;
static unsigned int screenHeight = 600;

GLFWwindow *createWindow()
{
    GLFWwindow *window;
    if (!glfwInit())
        return NULL;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    targetRefreshRate = mode->refreshRate;
    glfwWindowHint(GLFW_REFRESH_RATE, targetRefreshRate);
    targetFrameTime = 1.0 / targetRefreshRate;
    /*
    //Windowed Fullscreen
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    window = glfwCreateWindow(mode->width, mode->height, "Q", glfwGetPrimaryMonitor(), nullptr);
   */
    //screenWidth = mode->width;
    //screenHeight = mode->height;
    window = glfwCreateWindow(mode->width, mode->height, "Q", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

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

    // Windows, set scheduler granularity to 1ms
    timeBeginPeriod(1);

    return window;
}