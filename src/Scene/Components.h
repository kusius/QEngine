#pragma once

#include <string>
#include <vector>

#include <Thirdparty/glm/glm.hpp>
#include <Thirdparty/glm/gtc/matrix_transform.hpp>
#include <Graphics/Texture2D.h>
#include <Graphics/Shader.h>

enum Shape
{
    CUBE,
    SPRITE
};

struct TagComponent
{
    std::string Tag;
};

struct ShapeComponent
{
    Shape Shape = Shape::CUBE;
};

struct TransformComponent
{
    glm::vec3 Position    = {0.0f, 0.0f, 0.0f};
    glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Rotation    = {0.0f, 0.0f, 0.0f};
    glm::vec3 Scale       = {1.0f, 1.0f, 1.0f};
};

struct Render3DComponent
{
    std::string name; // friendly name given on creation
    std::string
        path; // the path of the descriptor file for the object (.gltf etc.)
    unsigned int id;
    unsigned int modelIndex;
    unsigned int instanceIndex;
    bool rayCastSelected;
};

struct RenderableComponent
{
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
};

struct Render3DComponentI
{
    unsigned int IndexPos;
    unsigned int IndexCount;

    unsigned int TexturePos;
    unsigned int TextureCount;

    unsigned int VertexPos;
    unsigned int NormalPos;
    unsigned int VertexCount;

    unsigned int TexCoordPos;
    unsigned int TexCoordCount;
};

struct SelectableComponent
{
    bool Selected;
};

struct BoundingBoxComponent
{
    glm::vec3 Min = {0.0f, 0.0f, 0.0f};
    glm::vec3 Max = {0.0f, 0.0f, 0.0f};
};