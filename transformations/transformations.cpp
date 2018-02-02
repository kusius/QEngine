#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Camera.h"
#include "ResourceManager.h"
#include "Graphics/SpriteRenderer.h"

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
float  lastX = (float) SCREEN_WIDTH / 2;
float lastY = (float) SCREEN_HEIGHT / 2 ; 






int main(int argc, char** argv)
{
	

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);



	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

	//glfw set 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	





	//OpenGL configuration
	//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//3D 
	//glm::mat4 model = glm::mat4(1);
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 view = glm::mat4(1);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / float(SCREEN_HEIGHT), 0.1f, 100.0f);

	

	

	//Make our shaders (read, compile, link)
	ResourceManager::LoadShader("shader.vert", "shader.frag", nullptr, "shader");
	ResourceManager::LoadTexture("container.jpg", GL_FALSE, "container");

	//3D set info to shader
	Shader shader = ResourceManager::GetShader("shader");
	//shader.SetMatrix4("model", model, GL_TRUE); //YOU NEED TO ACTIVATE SHADER
	shader.SetMatrix4("view", view, GL_TRUE);   //YOU NEED TO ACTIVATE SHADER
	shader.SetMatrix4("projection", projection);



	//Init all render data
	SpriteRenderer *Renderer;
	Renderer = new SpriteRenderer(ResourceManager::GetShader("shader"));

	//wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);




	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		

		
		processInput(window, deltaTime);
		//Render into backbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //solid black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / float(SCREEN_HEIGHT), 0.1f, 100.0f);
		view = camera.GetViewMatrix();

		
		for (unsigned int i = 0; i < 10; i++)
		{
			
			shader.SetMatrix4("view", view, GL_TRUE);
			shader.SetMatrix4("projection", projection);
			float angle = 20.0f * i;
			Renderer->DrawSprite(ResourceManager::GetTexture("container"), cubePositions[i], glm::vec2(1.0f, 1.0f), (float)glfwGetTime() + angle);
		}
		//Renderer->DrawSprite(ResourceManager::GetTexture("container"));


		glfwSwapBuffers(window);
		glfwPollEvents();
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