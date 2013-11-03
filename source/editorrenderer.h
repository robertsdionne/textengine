#ifndef TEXTENGINE_EDITORRENDERER_H_
#define TEXTENGINE_EDITORRENDERER_H_

#include <glm/glm.hpp>
#include <iostream>

#include "buffer.h"
#include "program.h"
#include "renderer.h"
#include "shader.h"
#include "vertexarray.h"
#include "vertexformat.h"

namespace textengine {

  class MeshEditor;
  class MeshRenderer;

  class EditorRenderer : public Renderer {
  public:
    EditorRenderer(MeshRenderer &mesh_renderer, MeshEditor &editor);

    virtual ~EditorRenderer() = default;

    virtual void Change(int width, int height) override;

    virtual void Create() override;

    virtual void Render() override;
    
  private:
    MeshRenderer &mesh_renderer;
    MeshEditor &editor;
    float inverse_aspect_ratio;

    Shader edge_geometry_shader, fragment_shader, point_geometry_shader, vertex_shader;
    Program edge_program, face_program, point_program;
    VertexFormat vertex_format;
    VertexArray pathfinding_edges_array, pathfinding_nodes_array, selected_face_array,
        selected_interior_edge_array, selected_exterior_edge_array, selected_point_array,
        move_indicator_array, selection_box_array;
    Buffer pathfinding_edges_buffer, pathfinding_nodes_buffer, selected_face_buffer,
        selected_interior_edge_buffer, selected_exterior_edge_buffer, selected_point_buffer,
        move_indicator_buffer, selection_box_buffer;

    glm::mat4 model_view, projection;
  };

}  // namespace textengine

#endif  // TEXTENGINE_EDITORRENDERER_H_
