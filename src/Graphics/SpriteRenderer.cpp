#include "SpriteRenderer.h"
#include <Managers/EntityManager.h>

Renderer::Renderer(Shader &shader)
{
	this->shader = &shader;
	this->initRenderData();
}

Renderer::Renderer(Shader &shader, Shader &highlightShader)
{
	this->highlightShader = &highlightShader;
	this->shader = &shader;
	this->initRenderData();
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &this->quadVAO);
}

void Renderer::initRenderData()
{
	GLuint VAO, VBO; // , EBO;

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	/*
 FOR SQUARE USE WITH VBO and glDrawElements
 */
	//float vertices[] = {
	//	//positions			//texture coordinates
	//	0.5f,  0.5f, 0.0f,  1.0f, 1.0f,// top right
	//	0.5f, -0.5f, 0.0f,  1.0f, 0.0f,// bottom right
	//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,// bottom left
	//	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f// top left
	//};

	//unsigned int indices[] = {  // note that we start from 0!
	//	0, 1, 3,  // first Triangle
	//	1, 2, 3   // second Triangle
	//};

	/*
 FOR CUBE USE DIRECTLY WITH glDrawArrays (no EBO)
 */

	float vertices[] = {
		// positions(3)          // normals(3)         // texture coords(2)

		//Back Face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		//Front Face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		//Left Face
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		//Right Face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		//Bottom
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		//Top
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};

	//generate
	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	//bind subsequent array commands to our VAO
	glBindVertexArray(this->quadVAO);

	//send vertices buffer to GPU
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//send element buffer to GPU
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	//specify structure of vertex array data
	//position : layout (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	//normal : layout (location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid *)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	//texture : layout (location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid *)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//we do NOT unbind EBO while VAO is still bound!!
	glBindVertexArray(0);
}
void Renderer::DrawCube(Texture2D &diffuseMap, Texture2D &specularMap, Texture2D &emissionMap,
						glm::vec3 translation, glm::vec2 scale, GLfloat rotation, GLboolean isSelected)
{
	//perform translations
	glm::mat4 transform = glm::mat4(1.0f);
	//translate
	transform = glm::translate(transform, translation);
	//scale
	transform = glm::scale(transform, glm::vec3(scale, 1.0f));
	//rotate
	transform = glm::rotate(transform, rotation * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	glActiveTexture(GL_TEXTURE0);
	diffuseMap.Bind();
	glActiveTexture(GL_TEXTURE1);
	specularMap.Bind();
	//glActiveTexture(GL_TEXTURE2);
	//emissionMap.Bind();

	//set the uniforms
	this->shader->Use();
	this->shader->SetMatrix4("model", transform);

	glBindVertexArray(this->quadVAO);
	if (isSelected)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0xFF);
		glDisable(GL_DEPTH_TEST);
		this->highlightShader->Use();
		this->highlightShader->SetMatrix4("model", glm::scale(transform, glm::vec3(1.05, 1.05, 1.05)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindVertexArray(0);
}

void Renderer::DrawPointLights(glm::vec3 pointLightPositions[], unsigned int numLights,
							   glm::vec3 scale, GLfloat rotation,
							   glm::vec3 color)
{
	for (unsigned int i = 0; i < numLights; i++)
	{
		glm::vec3 translation = pointLightPositions[i];
		//perform translations
		glm::mat4 transform = glm::mat4(1.0f);
		//translate
		transform = glm::translate(transform, translation);
		//scale
		transform = glm::scale(transform, glm::vec3(scale));
		//rotate
		transform = glm::rotate(transform, rotation * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		//set the uniforms
		this->shader->Use();
		this->shader->SetMatrix4("model", transform);

		glBindVertexArray(this->quadVAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
}

void Renderer::DrawSprite(Texture2D &texture)
{
	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	this->shader->Use();
	this->shader->SetVector4f("ourColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	glBindVertexArray(this->quadVAO);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(0);//no need to unbind every time but whaduheck
}

void Renderer::DrawEntities(const std::vector<Entity> &entities)
{
	for (Entity e : entities)
	{
		e.Draw(this->shader, this->highlightShader);
	}
}

void Renderer::DrawGameObjects()
{
	glBindVertexArray(VAO);
	GameObjects *gos = &(EntityManager::gameObjects);
	Texture *textureP = &(EntityManager::gameObjects.textures[0]);
	unsigned int meshIndex = 0;
	unsigned int textureIndex = 0;

	
	
	for (unsigned int i = 0; i < EntityManager::objectCount; i++)
	{
		shader->Use();
		shader->SetMatrix4("model", gos->modelMatrices[i]);
		
		// Draw each mesh of this object
		unsigned int numMeshes = gos->numMeshes[i];
		for (unsigned int j = 0; j < numMeshes; j++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int emissionNr = 1;
			unsigned int normalNr = 1;
			// Bind the appropriate textures for this mesh
			for (unsigned int k = 0; k < gos->numTextures[meshIndex]; k++)
			{
				glActiveTexture(GL_TEXTURE0 + k);
				string number;
				string name = gos->textures[textureIndex].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_emission")
					number = std::to_string(emissionNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);

				shader->Use();
				shader->SetInteger(("material." + name + number).c_str(), k);
				shader->SetFloat("material.shininess", 32.0f);
				glBindTexture(GL_TEXTURE_2D, gos->textures[textureIndex].id);
				textureIndex++;
			}
			glActiveTexture(GL_TEXTURE0);
			
		
			unsigned int numIndices = gos->numIndices[meshIndex];
			unsigned int baseIndex = 0;
			if (j > 0)
				baseIndex = gos->numIndices[j-1];
			unsigned int baseVertex = 0;
			if (j > 0)
				baseVertex = gos->numVertices[j-1];
			
			
			glDrawElementsBaseVertex(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT,
									(void*)(baseIndex*sizeof(unsigned int)), baseVertex);
			

			meshIndex++;
		}
	}
	glBindVertexArray(0);
}

void Renderer::SetupMeshes()
{
	unsigned int numVertices = EntityManager::gameObjects.vertices.size();
	unsigned int numIndices = EntityManager::gameObjects.indices.size();


	glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

	// bind a buffer and fill it with vertex data for ALL meshes (accross ALL gameobjects)
	glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &EntityManager::gameObjects.vertices[0], GL_STATIC_DRAW);

	// bind a buffer and fill it with index data for ALL meshes (accross ALL gameobjects)
	glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), &EntityManager::gameObjects.indices[0], GL_STATIC_DRAW);

    //vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    //vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    //texture ccordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    //tangent space
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));

    glBindVertexArray(0);
}