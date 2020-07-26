#ifndef RENDERED_ENTITY_H
#define RENDERED_ENTITY_H

#include "Graphics/SpriteRenderer.h"

enum RenderPrimitive
{
    Generic = 1 << 0,
    Cube = 1 << 1
};

class RenderedEntity
{
public:
    RenderedEntity(Renderer *renderer, RenderPrimitive primitive);
    void Draw();
    void SetSelected(GLboolean b);

private:
    RenderPrimitive primitive;
    Renderer *renderer;
    Texture2D diffuse, specular, emmission;
    glm::vec3 translation, scale;
    glm::mat4 model, view, projection;
    float rotation;
    GLboolean is_selected;
};

#endif