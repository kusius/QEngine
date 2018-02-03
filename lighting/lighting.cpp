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
float lastX = (float) SCREEN_WIDTH / 2;
float lastY = (float) SCREEN_HEIGHT / 2 ; 



//light position
glm::vec3 lightPos(1.2, 1.0f, 2.0f); //in global space coordinates
float rotateSpeed = 10.0f;


int main(int argc, char** argv)
{
	

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Phong Lighting", nullptr, nullptr);
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
	ResourceManager::LoadShader("shader.vert", "lightingshader.frag", nullptr, "shader");
	ResourceManager::LoadShader("lampshader.vert", "lampshader.frag", nullptr, "lampshader");
	ResourceManager::LoadTexture("container.jpg", GL_FALSE, "container");

	//3D set info to shader
	Shader shader = ResourceManager::GetShader("shader");
	Shader lightShader = ResourceManager::GetShader("lampshader");
	//shader.SetMatrix4("model", model, GL_TRUE); //YOU NEED TO ACTIVATE SHADER
	shader.SetMatrix4("view", view, GL_TRUE);   //YOU NEED TO ACTIVATE SHADER
	shader.SetVector3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f), GL_TRUE);
	shader.SetMatrix4("projection", projection);
	shader.SetVector3f("material.ambient", 0.2125f,	0.1275f,	0.054f);
	shader.SetVector3f("material.diffuse", 0.714f,	0.4284f,	0.18144f);
	shader.SetVector3f("material.specular", 0.393548f,	0.271906f,	0.166721f);
	shader.SetFloat("material.shininess", 25.6f);

	shader.SetVector3f("light.ambient", glm::vec3(1.0f)); //a dark ambient
	shader.SetVector3f("light.diffuse", glm::vec3(1.0f)); //darken the light a bit 
	shader.SetVector3f("light.specular", glm::vec3(1.0f)); //full white



	lightShader.SetMatrix4("view", view, GL_TRUE);   //YOU NEED TO ACTIVATE SHADER
	lightShader.SetMatrix4("projection", projection);
	
	
	


	//Init all render data
	Renderer *renderer, *lightRenderer;
	renderer = new Renderer(shader);
	lightRenderer = new Renderer(lightShader);

	//wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float lightDistance = glm::distance(glm::vec3(0.0f), lightPos);
	std::cout << lightDistance << std::endl;

	glm::vec3 lightColor;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		

		
		processInput(window, deltaTime);
		//Render into backbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / float(SCREEN_HEIGHT), 0.1f, 100.0f);
		view = camera.GetViewMatrix();

		//to move the light
		//lightPos.x = lightDistance * cos(rotateSpeed * glm::radians(glfwGetTime()));
		//lightPos.z = lightDistance *  sin(rotateSpeed * glm::radians(glfwGetTime()));

		

		
		


		shader.Use();
		shader.SetVector3f("viewPos", camera.Position);
		shader.SetVector3f("lightPos", lightPos);
		shader.SetMatrix4("view", view);
		shader.SetMatrix4("projection", projection);


		lightShader.SetMatrix4("view", view, GL_TRUE);
		lightShader.SetMatrix4("projection", projection);
			
		renderer->DrawSprite(glm::vec3(0.0f), glm::vec3(1.0f), 0.0f, glm::vec3(1.0f, 0.5f, 0.31f));


	
		lightRenderer->DrawSprite(lightPos, glm::vec3(0.2f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		


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