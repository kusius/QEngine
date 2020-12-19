#include "InstancedRenderer.h"

InstancedRenderer::InstancedRenderer(Shader &shader,
                                     GameObjectsInstanced *objects)
{
  this->gameObjectsInstanced = objects;
  this->shader = &shader;
  this->initRenderData();
}

void InstancedRenderer::initRenderData()
{
  if (this->gameObjectsInstanced == nullptr)
    return;

  unsigned int VBO, matrixBO;
  float quadVertices[] = {
      //	//positions			//texture coordinates
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };

  glGenVertexArrays(1, &this->quadVAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &matrixBO);

  glBindVertexArray(this->quadVAO);
  // send vertices
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);

  // layout (location = 0) positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT),
                        (void *)0);
  // layout (location = 2) texture coordinates
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT),
                        (void *)(3 * sizeof(GL_FLOAT)));

  // send modelmatrices
  size_t numInstances = this->gameObjectsInstanced->modelMatrices.size();
  glBindBuffer(GL_ARRAY_BUFFER, matrixBO);
  glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4),
               &this->gameObjectsInstanced->modelMatrices[0], GL_STATIC_DRAW);

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
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                        (void *)(2 * vec4Size));
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                        (void *)(3 * vec4Size));

  // specify the attribute divisor to update the vertex attribute once each
  // instance
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);
  glVertexAttribDivisor(7, 1);

  glBindVertexArray(0);
}

void InstancedRenderer::DrawGround()
{
  unsigned int quadIndices[] = {
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };
  glm::mat4 transform = glm::mat4(1.0f);
  this->shader->Use();
  this->shader->SetMatrix4("model", transform);
  glBindVertexArray(this->quadVAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &quadIndices);
  glBindVertexArray(0);
}