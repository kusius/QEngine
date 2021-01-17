#ifndef INSTANCED_RENDERER_H
#define INSTANCED_RENDERER_H

#include "Renderer.h"
#include <Objects/GameObjects.h>

class InstancedRenderer : public Renderer
{
public:
  /**
   * @brief Construct a new Instanced Renderer object
   *
   * @param shader compiled vertex and fragment shader to handle instanced
   * rendering
   * @param objects An intanced SOA that contains entities we want to draw
   */
  InstancedRenderer(Shader &shader, GameObjectsInstanced &objects);
  /**
   * @brief Draws all instances defined in the GameObjectsInstanced SOA
   * as quads
   *
   */
  void DrawAsQuads();
  void Draw();
  void SetupShaderData();

private:
  void initRenderData();
  GLuint quadVAO;
  GameObjectsInstanced &gameObjectsInstanced;
};

#endif // INSTANCED_RENDERER_H
