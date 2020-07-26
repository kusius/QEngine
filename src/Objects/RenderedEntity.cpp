#include "RenderedEntity.h"

void RenderedEntity::SetSelected(GLboolean b)
{
    this->is_selected = b;
}

RenderedEntity::RenderedEntity(Renderer *renderer, RenderPrimitive primitive)
{
    this->primitive = primitive;
    this->renderer = renderer;
    this->is_selected = GL_FALSE;
}

void RenderedEntity::Draw()
{
    switch (this->primitive)
    {
    case Cube:
        this->renderer->DrawCube(diffuse, specular, emmission, translation, scale, rotation, is_selected);
        break;
    case Generic:
        break;
    }
}