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
    std::vector<unsigned int> Indices;
    std::vector<unsigned int> TextureIds;
    std::vector<std::string> TextureType;
    std::vector<glm::vec3> VertexPos;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec3> Tangents;
    std::vector<glm::vec2> TexCoords;
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