#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>

/*----------------
 * SHADERS
 * --------------*/

//the vertex shader. Simple position shader 
const GLchar* vertexShaderSource = "#version 330 core			    \n"
"layout (location = 0) in vec3 position;			    \n"
"void main()							    \n"
"{								    \n"
"   gl_Position = vec4(position.x, position.y, position.z, 1.0);    \n"
"}								    \n\0";




//fragment shader. Orange color vertices 
const GLchar* fragmentShaderSourceOrng= "#version 330 core	    \n"
"out vec4 color;						    \n"
"void main()							    \n"
"{								    \n"
"       color = vec4(1.0f, 0.5f, 0.2f, 1.0f);			    \n"
"}								    \n\0";

//fragment shader. Yellow color vertices 
const GLchar* fragmentShaderSourceYellow= "#version 330 core	    \n"
"out vec4 color;						    \n"
"void main()							    \n"
"{								    \n"
"       color = vec4(1.0f, 1.0f, 0.0f, 1.0f);			    \n"
"}								    \n\0";




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

	int width, height; 

	glfwGetFramebufferSize(window, &width, &height);

	//first actual GL function, whose pointer is provided by GLEW
	//set the drawing viewport to be the the whole window size
	glViewport(0, 0, width, height);


	//coordinates of vertices to draw (triangle)
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};  
	GLfloat vertices_1[] = {
		// First triangle
		0.5f,  0.5f, 0.0f,  // Top Right
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f,  0.5f, 0.0f,  // Top Left 
		// Second triangle
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  // Bottom Left
		-0.5f,  0.5f, 0.0f   // Top Left
	};  
	GLfloat vertices_1_1[] = {

		// First triangle
		0.5f,  0.5f, 0.0f,  // Top Right
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f,  0.5f, 0.0f,  // Top Left 

	};
	GLfloat vertices_1_2[] = {

		// Second triangle
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  // Bottom Left
		-0.5f,  0.5f, 0.0f   // Top Left

	};


	//compile the shaders at runtime 
	GLuint vertexShader, fragmentShader[2];
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentShader[1] = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShader[0], 1, &fragmentShaderSourceOrng, NULL);
	glShaderSource(fragmentShader[1], 1, &fragmentShaderSourceYellow, NULL);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader[0]);
	glCompileShader(fragmentShader[1]);

	/*----------------
	 * SHADER PROGRAM
	 * --------------*/

	GLuint shaderProgram[2];

	//shader program with Oragne frament shader
	shaderProgram[0] = glCreateProgram();
	glAttachShader(shaderProgram[0], vertexShader);
	glAttachShader(shaderProgram[0], fragmentShader[0]);
	glLinkProgram(shaderProgram[0]);
	//shader program with Yellow frament shader
	shaderProgram[1] = glCreateProgram();
	glAttachShader(shaderProgram[1], vertexShader);
	glAttachShader(shaderProgram[1], fragmentShader[1]);
	glLinkProgram(shaderProgram[1]);
	//delete shaders. unneeded from now on
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader[0]);  
	glDeleteShader(fragmentShader[1]);  


	/*----------------
	 * VERTEX ARRAY OBJECT AND VERTEX BUFFER OBJECT
	 * --------------*/





	//generate a VAO,VBO
	GLuint VAO[2],VBO[2];
	glGenBuffers(2, VBO); 
	glGenVertexArrays(2, VAO);
	//first triangle
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1_1), vertices_1_1, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); 
	glBindVertexArray(0);

	//second triangle
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1_2), vertices_1_2, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); 
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



		glUseProgram(shaderProgram[0]);
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(shaderProgram[1]);
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}


	glfwTerminate();
	return 0;
}
