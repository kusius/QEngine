#include "Mesh.h"
#include <iostream>

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    : vertices(vertices), indices(indices), textures(textures), selected(GL_FALSE)
{
    setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // vertex buffer
    glGenBuffers(1, &EBO); // element (index) buffer

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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

void Mesh::Draw(Shader *shader, Shader *highlightsShader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int emissionNr = 1;
    unsigned int normalNr = 1;
    //bind all the appropriate textures
    //note that we have a naming convention:
    //spexcular texture 1 is named: texure_specular1
    //and the same for other specular and diffuse textures
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "texture_emission")
            number = std::to_string(emissionNr++);
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);

        shader->Use();
        shader->SetInteger(("material." + name + number).c_str(), i);
        shader->SetFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    //draw mesh
    glBindVertexArray(VAO);
    //draw outline
    if (highlightsShader)
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0xFF);
        glDisable(GL_DEPTH_TEST);
        highlightsShader->Use();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}