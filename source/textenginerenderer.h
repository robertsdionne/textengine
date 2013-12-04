#ifndef TEXTENGINE_TEXTENGINERENDERER_H_
#define TEXTENGINE_TEXTENGINERENDERER_H_

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

  class Mouse;
  class Scene;
  class Updater;

  class TextEngineRenderer : public Renderer {
  public:
    TextEngineRenderer(Mouse &mouse, Updater &updater, Scene &scene);

    virtual ~TextEngineRenderer() = default;

    virtual void Change(int width, int height) override;

    virtual void Create() override;

    virtual void Render() override;

  private:

    void DrawAxisAlignedBoundingBox(AxisAlignedBoundingBox aabb);

    void DrawCircle(glm::vec2 center, float radius);

    void DrawLine(glm::vec2 begin, glm::vec2 end);

    void DrawLines();

    void DrawRectangle(glm::vec2 center, glm::vec2 dimensions);

    void PopMatrix();

    void PushMatrix();
    
  private:
    Mouse &mouse;
    Updater &updater;
    Scene &scene;
    float inverse_aspect_ratio;

    Shader edge_geometry_shader, fragment_shader, point_geometry_shader, vertex_shader;
    Program edge_program, face_program, point_program;
    VertexFormat vertex_format;
    VertexArray circle_array, circle_edge_array, rectangle_array, rectangle_edge_array;
    VertexArray line_array;
    Buffer circle_buffer, circle_edge_buffer, rectangle_buffer, rectangle_edge_buffer;
    Buffer line_buffer;

    glm::mat4 model_view, projection;

    glm::vec4 fill, stroke;
    float stroke_width;

    std::vector<glm::mat4> matrix_stack;

    Drawable unit_circle, unit_circle_border, unit_square, unit_square_border, lines;

    int width, height, scroll0;
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
