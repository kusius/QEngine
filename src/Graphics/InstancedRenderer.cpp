#include "InstancedRenderer.h"
#include <Managers/EntityManager.h>

InstancedRenderer::InstancedRenderer(Shader &shader,
                                     GameObjectsInstanced &objects)
    : gameObjectsInstanced(objects)
{
  this->shader = &shader;
  if (gameObjectsInstanced.modelMatrices.size() > 0)
    this->SetupShaderData();
}

void InstancedRenderer::SetupShaderData()
{
  unsigned int numVertices = this->gameObjectsInstanced.vertices.size();
  unsigned int numIndices = this->gameObjectsInstanced.indices.size();

  unsigned int VBO, matrixBO, EBO;

  glGenVertexArrays(1, &this->quadVAO);
  glBindVertexArray(this->quadVAO);

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &matrixBO);
  glGenBuffers(1, &EBO);

  // send vertices
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex),
               &this->gameObjectsInstanced.vertices[0], GL_STATIC_DRAW);

  // send indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t),
               &this->gameObjectsInstanced.indices[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // texture ccordinates
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));
  // tangent space
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Tangent));

  // model matrices
  size_t numInstances = this->gameObjectsInstanced.modelMatrices.size();
  glBindBuffer(GL_ARRAY_BUFFER, matrixBO);
  glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4),
               &this->gameObjectsInstanced.modelMatrices[0], GL_STATIC_DRAW);

  // layout (location = 4) instanced model matrices
  // the columns of the model are in locations 4,5,6,7 (because OpenGL accepts
  // up to vec4 for a single vertex attribute)
  unsigned int vec4Size = sizeof(glm::vec4);

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(0));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                        (void *)(1 * vec4Size));

  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                        (void *)(2 * vec4Size));

  glEnableVertexAttribArray(7);
  glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                        (void *)(3 * vec4Size));

  // specify the attribute divisor to update the vertex attribute once each
  // instance
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);
  glVertexAttribDivisor(7, 1);

  glBindVertexArray(0);
}

void InstancedRenderer::DrawAsQuads()
{
  unsigned int quadIndices[] = {
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };
  unsigned int textureIndex = 0;
  bindTextures(gameObjectsInstanced.numTextures, gameObjectsInstanced.textures,
               textureIndex);

  this->shader->Use();
  glm::mat4 transform = glm::mat4(1.0f);
  unsigned int instanceCount = EntityManager::terrain.modelMatrices.size();
  glBindVertexArray(this->quadVAO);
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &quadIndices,
                          instanceCount);

  glBindVertexArray(0);
}

void InstancedRenderer::Draw()
{
  unsigned int textureIndex = 0;
  bindTextures(this->gameObjectsInstanced.numTextures,
               this->gameObjectsInstanced.textures, textureIndex);

  this->shader->Use();
  glm::mat4 transform = glm::mat4(1.0f);
  unsigned int instanceCount = this->gameObjectsInstanced.modelMatrices.size();

  glBindVertexArray(this->quadVAO);
  glDrawElementsInstanced(
      GL_TRIANGLES, this->gameObjectsInstanced.indices.size(), GL_UNSIGNED_INT,
      (void *)this->gameObjectsInstanced.indices[0], instanceCount);
  glBindVertexArray(0);
}