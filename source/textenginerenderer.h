#ifndef TEXTENGINE_TEXTENGINERENDERER_H_
#define TEXTENGINE_TEXTENGINERENDERER_H_

#include <glm/glm.hpp>
#include <gltext.hpp>
#include <iostream>

#include "buffer.h"
#include "program.h"
#include "renderer.h"
#include "shader.h"
#include "shaders.h"
#include "vertexarray.h"
#include "vertexformat.h"

namespace textengine {

  class SubjectiveMeshRenderer;
  class Updater;

  class TextEngineRenderer : public Renderer {
  public:
    TextEngineRenderer(Updater &updater, SubjectiveMeshRenderer &mesh_renderer);

    virtual ~TextEngineRenderer() = default;

    virtual void Change(int width, int height) override;

    virtual void Create() override;

    virtual void Render() override;
    
  private:
    Updater &updater;
    SubjectiveMeshRenderer &mesh_renderer;
    float inverse_aspect_ratio;

    Shader edge_geometry_shader, fragment_shader, point_geometry_shader, vertex_shader;
    Program edge_program, face_program, point_program;
    VertexFormat vertex_format;
    VertexArray player_array, player_edge_array, player_view_array, shots_array, npc_array, npc_edge_array;
    Buffer player_buffer, player_edge_buffer, player_view_buffer, shots_buffer, npc_buffer, npc_edge_buffer;

    glm::mat4 model_view, projection;
    gltext::Font font;
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
