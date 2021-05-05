/**
 * @file SpriteRenderer.h
 * @author GMK
 * @brief The renderer is a system that contains functions with the draw
 * commands of the various drawables. The @see EntityManager gameObjects SOA as
 * well as any primitive geometry.
 *
 * Primitive 3D drawables are: cubes
 *
 */
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "Texture2D.h"
#include "Shader.h"

#include <Thirdparty/glad/glad.h>
#include <Thirdparty/glm/glm.hpp>
#include <Thirdparty/glm/gtc/matrix_transform.hpp>
#include <Graphics/Model.h>
#include <Objects/GameObjects.h>
#include <Graphics/Shader.h>
#include <Graphics/Model.h>
#include <Graphics/Shader.h>
#include <Graphics/Camera.h>
#include <Scene/Components.h>

#include <vector>

class Renderer
{
  public:
    // constructor inits shaders
    Renderer();
    Renderer(Shader &shader);
    Renderer(Shader &shader, Shader &highlightShader, Shader &unicolorShader);
    // Destructor
    ~Renderer();

    void BeginFrame(const Camera &camera);

    void EndFrame();

    // Renders a defined quad textured with given sprite
    void DrawCube(Texture2D &diffuseMap, Texture2D &specularMap,
                  Texture2D &emissionMap,
                  glm::vec3 translation = glm::vec3(0.0f),
                  glm::vec2 scale       = glm::vec3(1.0f, 1.0f, 1.0f),
                  GLfloat rotation = 0.0f, GLboolean isSelected = GL_FALSE);
    void DrawCube(glm::vec3 position, glm::vec3 scale, GLfloat rotation,
                  glm::vec3 color);

    void DrawShape(TransformComponent transform, ShapeComponent shape,
                   RenderableComponent renderable);
    void DrawBoundingBox(const BoundingBox &bbox,
                         GLboolean wireframe = GL_FALSE);
    void DrawPointLights(glm::vec3 scale  = glm::vec3(1.0f, 1.0f, 1.0f),
                         GLfloat rotation = 0.0f,
                         glm::vec3 color  = glm::vec3(1.0f));
    void DrawSprite(Texture2D &texture);

    /**
     * @brief Draws the EntityManager::gameObjects SOA.
     * @see EntityManager
     */
    void DrawGameObjects();

    /**
     * @brief Prepare the vertex arrays and shader uniforms for rendering
     * the SOA defined in @see EntityManager.
     */
    void SetupShaderData();

    Shader *shader;
    Shader *highlightShader;
    Shader *unicolorShader;

  protected:
    /**
     * @brief Bind the texture uniforms needed for our gameObject.frag fragment
     * shader
     *
     * @param numTextures how many textures to bind to our shader
     * @param textures the array from which to draw the Texture objects
     * @param textureIndex (in/out) an index to use for the array (we increment
     * it inside this function). This is in case the calling code has a running
     * index for the textures array.
     */
    inline void bindTextures(unsigned int numTextures,
                             const std::vector<Texture> &textures,
                             unsigned int &textureIndex);

  private:
    GLuint cubeVAO;
    GLuint mVAO, mEBO, mVBO;
    // Inits and configures the quad's buffer and vertex attributes
    void initRenderData();
};
#endif
