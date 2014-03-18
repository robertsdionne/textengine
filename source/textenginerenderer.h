#ifndef __textengine__textenginerenderer__
#define __textengine__textenginerenderer__

#include <chrono>
#include <glm/glm.hpp>
#include <iostream>

#include "buffer.h"
#include "drawable.h"
#include "program.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"
#include "shaders.h"
#include "vertexarray.h"
#include "vertexformat.h"

namespace textengine {
  
  class Controller;
  class Mouse;
  class Scene;

  class TextEngineRenderer : public Renderer {
  public:
    TextEngineRenderer(Mouse &mouse, Controller &updater, Scene &scene, bool edit);

    virtual ~TextEngineRenderer() = default;

    virtual void Change(int width, int height) override;

    virtual void Create() override;

    virtual void Render() override;

  private:

    void DrawAxisAlignedBoundingBox(AxisAlignedBoundingBox aabb);

    void DrawCircle(glm::vec2 center, float radius);

    void DrawRectangle(glm::vec2 center, glm::vec2 dimensions);

    void PopMatrix();

    void PushMatrix();
    
    void MaybeRebuildAttenuationShader();
    
  private:
    Mouse &mouse;
    Controller &updater;
    Scene &scene;
    bool edit;
    float inverse_aspect_ratio;

    Shader attenuation_fragment_shader, attenuation3_fragment_shader;
    Shader attenuation_vertex_shader, fragment_shader, vertex_shader;
    size_t attenuation_fragment_shader_source_hash;
    Program attenuation_program, attenuation3_program;
    Program face_program;
    VertexFormat vertex_format;
    VertexArray attenuation_array, circle_array, rectangle_array;
    Buffer attenuation_buffer, circle_buffer, rectangle_buffer;
    glm::mat4 model_view, projection;
    AttenuationShaderTemplate attenuation_template;
    Attenuation3ShaderTemplate attenuation3_template;

    glm::vec4 fill, stroke;
    float stroke_width;

    std::vector<glm::mat4> matrix_stack;

    Drawable attenuation, unit_circle, unit_square;

    int width, height, scroll0;
  };

}  // namespace textengine

#endif /* defined(__textengine__textenginerenderer__) */
