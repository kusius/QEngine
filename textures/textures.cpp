#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <soil.h>
#include <stdio.h>
#include "shader.h"



const GLuint WIDTH = 800 , HEIGHT = 600;
const float increaseRate = 0.01f;
float mixRatio = 0.2f;
bool repeatKeyUp = false;
bool repeatKeyDown = false;
float rate = 0.0f;
//key_callback function to use the escape key to set the window to close
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else if (key == GLFW_KEY_UP &&  action == GLFW_PRESS)
	{
		repeatKeyUp = true;
		rate = 0.0f;
		if(mixRatio + increaseRate <= 1.0f) mixRatio += increaseRate;
		/*
		//printf("up key\n");
		if(mixRatio + increaseRate <= 1.0f) mixRatio += increaseRate;
		*/
	}
	else if (key == GLFW_KEY_UP && (action == GLFW_RELEASE))
	{
		repeatKeyUp = false;
	}
	else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		repeatKeyDown = true;
		//printf("down key\n");
		if (mixRatio -increaseRate >= 0.0f) mixRatio -= increaseRate; 
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
	{
		repeatKeyDown = false;
	}
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


	//used to get the framebuffer size of given window
	//glfwGetFramebufferSize(window, &width, &height);

	//first actual GL function, whose pointer is provided by GLEW
	//set the drawing viewport to be the the whole window size
	glViewport(0, 0, WIDTH, HEIGHT);



	Shader shader("C:/OpenGL/learnopengl/textures/shader.vs", "C:/OpenGL/learnopengl/textures/shader.frag");

	 // Set up vertex data (and buffer(s)) and attribute pointers
	    GLfloat vertices[] = {
		// Positions          // Colors           // Texture Coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
	    };

	    GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	    };

	/*----------------
	 * VERTEX ARRAY OBJECT VERTEX BUFFER OBJECT AND ELEMENT BUFFER OBJECT
	 * --------------*/

	//generate a VAO,VBO
	GLuint VAO,VBO,EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); 
	glGenBuffers(1, &EBO);
	


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//position attribute 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//color attribute 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//texcoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	glBindVertexArray(0);

	/*----------------
	 * TEXTURE
	 * --------------*/

	// Load and create a texture 
	GLuint texture, texture1;
	glGenTextures(1, &texture);
	glGenTextures(1, &texture1);

	

	/*FIRST TEXTURE*/
	//optional: set the active right before binding TEXTURE UNIT if 
	//multiple textures are wanted. Default is GL_TEXURE0 and 
	//does not need a glUniform1f() call 
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Clamp first texture to edge 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	int twidth, theight;
	unsigned char* image = SOIL_load_image("C:/OpenGL/learnopengl/textures/container.jpg", &twidth, &theight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	
	/*SECOND TEXTURE*/
	//optional: set the active right before binding TEXTURE UNIT if 
	//multiple textures are wanted. Default is GL_TEXURE0 and 
	//does not need a glUniform1f() call 
	glBindTexture(GL_TEXTURE_2D, texture1); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	image = SOIL_load_image("C:/OpenGL/learnopengl/textures/awesomeface.png", &twidth, &theight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.


	/*----------------
	 * MAIN GAME LOOP
	 * --------------*/
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		rate += 0.02f;
		if (repeatKeyUp && rate>=3.0f )
		{
			rate = 0.0f;
			if(mixRatio + increaseRate <= 1.0f) mixRatio += increaseRate;
		}

		else if (repeatKeyDown && rate>=3.0f )
		{
			rate = 0.0f;
			if (mixRatio - increaseRate >= 0.0f) mixRatio -= increaseRate; 
		}

		//clear the color of the buffer to blue-ish
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindTexture(GL_TEXTURE_2D, texture);

		shader.Use();

		// Bind Textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture1"), 1);  

		//set the mix ratio of the 2 textures 
		glUniform1f(glGetUniformLocation(shader.Program, "mixRatio"), mixRatio);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}


	//deallocate all resources .
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}
