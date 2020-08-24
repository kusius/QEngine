#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#define MAX_POINT_LIGHTS 4
#define MAX_GAME_OBJECTS 512

#include <vector>

#include "Thirdparty/glm/glm.hpp"
#include "Graphics/Texture.h"

struct GameObjects
{
    unsigned int nPointLights = MAX_POINT_LIGHTS;
    glm::vec3 pointLightPositions[MAX_POINT_LIGHTS];
    
    std::vector<Texture2D> textures;
    std::vector<glm::mat4> modelTransforms;
};

#endif