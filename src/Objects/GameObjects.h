#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#define MAX_POINT_LIGHTS 4
#define MAX_GAME_OBJECTS 512

#include "Thirdparty/glm/glm.hpp"
#include "Graphics/Mesh.h"
#include <vector>

struct Lights
{
    unsigned int nPointLights = MAX_POINT_LIGHTS;
    glm::vec3 pointLightPositions[MAX_POINT_LIGHTS];
};

struct GameObjects
{
    std::vector<unsigned int> numMeshes;
    std::vector<unsigned int> numTextures;
    std::vector<unsigned int> numVertices;
    std::vector<unsigned int> numIndices;

    /*Geometry*/
    std::vector<glm::mat4> modelMatrices;
    /*Mesh data (can we just send everything to the GPU at once? vertices+indices and textures)*/
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
};

#endif //GAME_OBJECTS_H