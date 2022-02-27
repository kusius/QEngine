#ifndef MESH_H
#define MESH_H

// A 3D mesh storage and rednering class

#include <Thirdparty/glm/glm.hpp>
#include <Thirdparty/glad/glad.h>

#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

struct BoundingBox
{
    glm::vec3 mMin;
    glm::vec3 mMax;
};

class Mesh
{

  public:
    /*Mesh Data*/
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    BoundingBox boundingBox;
    /*Functions*/
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices,
         vector<Texture> textures, BoundingBox boundingBox);
    void Draw(Shader *shader, Shader *highlightShader = nullptr);
    /*Other data*/
    GLboolean selected;

  private:
    /*Render data*/
    unsigned int VAO, VBO, EBO;
    /*Functions*/
    void setupMesh();
};

#endif
