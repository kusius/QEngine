#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "shader.h"

//key_callback function to use the escape key to set the window to close
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main () 
{
	//GLFW for creating a window and a viewport to draw in 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800,600,"LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) 
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//register the function handling keypresses
	glfwSetKeyCallback(window, key_callback);


	//GLEW for GL function pointers
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	/*----------------
	 * SHADERS
	 * --------------*/

	Shader shader("C:/OpenGL/learnopengl/shaders/shader.vs", "C:/OpenGL/learnopengl/shaders/shader.frag");

	int width, height; 

	glfwGetFramebufferSize(window, &width, &height);

	//first actual GL function, whose pointer is provided by GLEW
	//set the drawing viewport to be the the whole window size
	glViewport(0, 0, width, height);


	//coordinates of vertices to draw (triangle)
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};  


	/*----------------
	 * VERTEX ARRAY OBJECT AND VERTEX BUFFER OBJECT
	 * --------------*/





	//generate a VAO,VBO
	GLuint VAO,VBO;
	glGenBuffers(1, &VBO); 
	glGenVertexArrays(1, &VAO);
	//first triangle
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); 
	glBindVertexArray(0);


	/*----------------
	 * MAIN GAME LOOP
	 * --------------*/

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//clear the color of the buffer to blue-ish
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		shader.Use();
#if 0
		//change colors with time
		GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		GLfloat blueValue = (cos(timeValue) / 2) + 0.5;
		GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "vertexColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, blueValue, 1.0f);
#endif


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}


	glfwTerminate();
	return 0;
}
