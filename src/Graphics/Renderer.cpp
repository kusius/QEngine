#include "Renderer.h"
#include <Managers/EntityManager.h>

Renderer::Renderer() {}

Renderer::Renderer(Shader &shader)
{
    this->highlightShader = nullptr;
    this->shader          = &shader;
    this->initRenderData();
}

Renderer::Renderer(Shader &shader, Shader &highlightShader)
{
    this->highlightShader = &highlightShader;
    this->shader          = &shader;
    this->initRenderData();
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &this->cubeVAO);
    glDeleteVertexArrays(1, &this->mVAO);
}

void Renderer::initRenderData()
{
    GLuint VBO;

    float cubeVertices[] = {
        // positions(3)          // normals(3)         // texture coords(2)

        // Back Face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f,
        0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, -0.5f,
        -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f,
        0.0f, -1.0f, 0.0f, 1.0f,
        // Front Face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f,
        // Left Face
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f,
        -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -0.5f,
        -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, -1.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        // Right Face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f,
        1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f,

        // Bottom
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f,
        0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -0.5f,
        -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f,
        -1.0f, 0.0f, 0.0f, 1.0f,
        // Top
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f,
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f,
        -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f};

    // generate
    glGenVertexArrays(1, &this->cubeVAO);
    glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO);

    // bind subsequent array commands to our VAO
    glBindVertexArray(this->cubeVAO);

    // send vertices buffer to GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_STATIC_DRAW);

    // send element buffer to GPU
    /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  GL_STATIC_DRAW);*/

    // specify structure of vertex array data
    // position : layout (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
                          (GLvoid *)0);

    // normal : layout (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
                          (GLvoid *)(3 * sizeof(GL_FLOAT)));
    // texture : layout (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
                          (GLvoid *)(6 * sizeof(GL_FLOAT)));

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // we do NOT unbind EBO while VAO is still bound!!
    glBindVertexArray(0);
}
void Renderer::DrawCube(Texture2D &diffuseMap, Texture2D &specularMap,
                        Texture2D &emissionMap, glm::vec3 translation,
                        glm::vec2 scale, GLfloat rotation, GLboolean isSelected)
{
    // perform translations
    glm::mat4 transform = glm::mat4(1.0f);
    // translate
    transform = glm::translate(transform, translation);
    // scale
    transform = glm::scale(transform, glm::vec3(scale, 1.0f));
    // rotate
    transform = glm::rotate(transform, rotation * glm::radians(50.0f),
                            glm::vec3(0.5f, 1.0f, 0.0f));

    glActiveTexture(GL_TEXTURE0);
    diffuseMap.Bind();
    glActiveTexture(GL_TEXTURE1);
    specularMap.Bind();
    // glActiveTexture(GL_TEXTURE2);
    // emissionMap.Bind();

    // set the uniforms
    this->shader->Use();
    this->shader->SetMatrix4("model", transform);

    glBindVertexArray(this->cubeVAO);
    if (isSelected)
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0xFF);
        glDisable(GL_DEPTH_TEST);
        this->highlightShader->Use();
        this->highlightShader->SetMatrix4(
            "model", glm::scale(transform, glm::vec3(1.05, 1.05, 1.05)));
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

void Renderer::DrawCube(glm::vec3 position, glm::vec3 scale, GLfloat rotation,
                        glm::vec3 color)
{
    // perform translations
    glm::mat4 transform = glm::mat4(1.0f);
    // translate
    transform = glm::translate(transform, position);
    // scale
    transform = glm::scale(transform, glm::vec3(scale));
    // rotate
    transform = glm::rotate(transform, rotation * glm::radians(50.0f),
                            glm::vec3(0.5f, 1.0f, 0.0f));

    // set the uniforms
    this->shader->Use();
    this->shader->SetMatrix4("model", transform);

    glBindVertexArray(this->cubeVAO);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Renderer::DrawBoundingBox(const BoundingBox &bbox, GLboolean wireframe)
{
    // 1. calculate size (scale)
    glm::vec3 scale =
        glm::vec3(bbox.mMax.x - bbox.mMin.x, bbox.mMax.y - bbox.mMin.y,
                  bbox.mMax.z - bbox.mMin.z);
    // 2. calculate center (position)
    glm::vec3 center = glm::vec3((bbox.mMin.x + bbox.mMax.x) / 2,
                                 (bbox.mMin.y + bbox.mMax.y) / 2,
                                 (bbox.mMin.z + bbox.mMax.z) / 2);

    // 3. rotation is always zero (axis aligned)
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    DrawCube(center, scale, 0.0f, glm::vec3(1.0f));
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::DrawPointLights(glm::vec3 pointLightPositions[],
                               unsigned int numLights, glm::vec3 scale,
                               GLfloat rotation, glm::vec3 color)
{
    for (unsigned int i = 0; i < numLights; i++)
    {
        glm::vec3 translation = pointLightPositions[i];
        // perform translations
        glm::mat4 transform = glm::mat4(1.0f);
        // translate
        transform = glm::translate(transform, translation);
        // scale
        transform = glm::scale(transform, glm::vec3(scale));
        // rotate
        transform = glm::rotate(transform, rotation * glm::radians(50.0f),
                                glm::vec3(0.5f, 1.0f, 0.0f));

        // set the uniforms
        this->shader->Use();
        this->shader->SetMatrix4("model", transform);

        glBindVertexArray(this->cubeVAO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

    glBindVertexArray(this->cubeVAO);
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // glBindVertexArray(0);//no need to unbind every time but whaduheck
}

void Renderer::DrawGameObjects()
{
    glBindVertexArray(mVAO);
    GameObjects *gos          = &(EntityManager::gameObjects);
    unsigned int meshIndex    = 0;
    unsigned int textureIndex = 0;
    unsigned int baseIndex    = 0;
    unsigned int baseVertex   = 0;

    // TODO: Draw instances with glDraw*Instanced (?)
    for (unsigned int i = 0; i < EntityManager::gameObjects.numMeshes.size();
         i++)
    {
        // Draw each mesh of this object
        unsigned int numMeshes = gos->numMeshes[i];

        for (unsigned int j = 0; j < numMeshes; j++)
        {
            // Bind the appropriate textures for this mesh
            bindTextures(gos->numTextures[meshIndex], gos->textures,
                         textureIndex);

            unsigned int numIndices = gos->numIndices[meshIndex];

            for (unsigned int instance = 0;
                 instance < gos->modelMatrices[i].size(); instance++)
            {
                shader->Use();
                shader->SetMatrix4("model", gos->modelMatrices[i][instance]);
                if ((gos->flags[i][instance] & FLAG_SELECTED) &&
                    highlightShader)
                {
                    glStencilFunc(GL_ALWAYS, 1, 0xFF);
                    glStencilMask(0xFF);
                    glDrawElementsBaseVertex(
                        GL_TRIANGLES, numIndices, GL_UNSIGNED_INT,
                        (void *)(baseIndex * sizeof(uint32_t)), baseVertex);

                    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                    glStencilMask(0xFF);
                    glDisable(GL_DEPTH_TEST);
                    highlightShader->Use();
                    highlightShader->SetMatrix4(
                        "model", glm::scale(gos->modelMatrices[i][instance],
                                            glm::vec3(1.01, 1.01, 1.01)));
                    glDrawElementsBaseVertex(
                        GL_TRIANGLES, numIndices, GL_UNSIGNED_INT,
                        (void *)(baseIndex * sizeof(uint32_t)), baseVertex);
                    glStencilMask(0xFF);
                    glStencilFunc(GL_ALWAYS, 1, 0xFF);
                    glEnable(GL_DEPTH_TEST);
                }
                else
                    glDrawElementsBaseVertex(
                        GL_TRIANGLES, numIndices, GL_UNSIGNED_INT,
                        (void *)(baseIndex * sizeof(uint32_t)), baseVertex);
            }

            baseIndex += gos->numIndices[meshIndex];
            baseVertex += gos->numVertices[meshIndex];

            meshIndex++;
        }
    }
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

inline void Renderer::bindTextures(unsigned int numTextures,
                                   const std::vector<Texture> &textures,
                                   unsigned int &textureIndex)
{
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int emissionNr = 1;
    unsigned int normalNr   = 1;

    for (unsigned int k = 0; k < numTextures; k++)
    {
        glActiveTexture(GL_TEXTURE0 + k);
        string number;
        string name = textures[textureIndex].type;
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
        glBindTexture(GL_TEXTURE_2D, textures[textureIndex].id);
        textureIndex++;
    }
    glActiveTexture(GL_TEXTURE0);
}

void Renderer::SetupShaderData()
{
    unsigned int numVertices = EntityManager::gameObjects.vertices.size();
    unsigned int numIndices  = EntityManager::gameObjects.indices.size();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // bind a buffer and fill it with vertex data for ALL meshes (accross ALL
    // gameobjects)
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex),
                 &EntityManager::gameObjects.vertices[0], GL_STATIC_DRAW);

    // bind a buffer and fill it with index data for ALL meshes (accross ALL
    // gameobjects)
    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t),
                 &EntityManager::gameObjects.indices[0], GL_STATIC_DRAW);

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

    glBindVertexArray(0);
}