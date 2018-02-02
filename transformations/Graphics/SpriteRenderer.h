#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <GL/glew.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Shader.h"

class SpriteRenderer
{
public:
	//constructor inits shaders/shapes
	SpriteRenderer(Shader &shader);
	//Destructor
	~SpriteRenderer();
	//Renders a defined quad textured with given sprite
	void DrawSprite(Texture2D &texture, glm::vec3 translation = glm::vec3(0.0f) ,
		glm::vec2 scale = glm::vec2(1.0f, 1.0f), GLfloat rotation = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));

	void DrawSprite(Texture2D & texture);


private:
	//Render state
	Shader shader;
	GLuint quadVAO;
	//Inits and configures the quad's buffer and vertex attributes
	void initRenderData();

};

#endif

