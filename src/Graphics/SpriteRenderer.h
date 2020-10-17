/**
 * @file SpriteRenderer.h
 * @author GMK
 * @brief The renderer is a system that contains functions with the draw commands of
 * the various drawables. The @see EntityManager gameObjects SOA as well as any primitive geometry.
 * 
 * Primitive 3D drawables are: cubes
 * 
 */
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <vector>

#include "Thirdparty/glad/glad.h"
#include "Thirdparty/glm/glm.hpp"
#include "Thirdparty/glm/gtc/matrix_transform.hpp"

#include "Objects/Entity.h"
#include "Texture.h"
#include "Shader.h"

class Renderer
{
public:
	//constructor inits shaders/shapes
	Renderer();
	Renderer(Shader &shader);
	Renderer(Shader &shader, Shader &highlightShader);
	//Destructor
	~Renderer();
	//Renders a defined quad textured with given sprite
	void DrawCube(Texture2D &diffuseMap, Texture2D &specularMap, Texture2D &emissionMap, glm::vec3 translation = glm::vec3(0.0f),
				  glm::vec2 scale = glm::vec3(1.0f, 1.0f, 1.0f), GLfloat rotation = 0.0f, GLboolean isSelected = GL_FALSE);

	void DrawPointLights(glm::vec3 pointLightPositions[], unsigned int numLights,
						 glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), GLfloat rotation = 0.0f,
						 glm::vec3 color = glm::vec3(1.0f));
	void DrawSprite(Texture2D &texture);

	void DrawEntities(const std::vector<Entity> &entities);

	/**
	 * @brief Draws the EntityManager::gameObjects SOA. 
	 * @see EntityManager
	 * 
	 */
	void DrawGameObjects();
	void SetupMeshes();

private:
	//Render state
	Shader *shader;
	Shader *highlightShader;
	GLuint quadVAO;
	GLuint VAO, EBO, VBO;
	//Inits and configures the quad's buffer and vertex attributes
	void initRenderData();
};

#endif
