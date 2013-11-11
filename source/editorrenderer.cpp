#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "checks.h"
#include "editorrenderer.h"
#include "gamestate.h"
#include "mesheditor.h"
#include "meshrenderer.h"
#include "shaders.h"
#include "updater.h"

namespace textengine {


  EditorRenderer::EditorRenderer(MeshRenderer &mesh_renderer, MeshEditor &editor)
  : mesh_renderer(mesh_renderer), editor(editor), model_view(glm::mat4()),
  projection(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)) {}

  void EditorRenderer::Change(int width, int height) {
    glViewport(0, 0, width, height);
    inverse_aspect_ratio = static_cast<float>(height) / static_cast<float>(width);
    projection = glm::ortho(-1.0f, 1.0f, -inverse_aspect_ratio, inverse_aspect_ratio, -1.0f, 1.0f);
    editor.set_model_view_projection(projection * model_view);
    mesh_renderer.Change(width, height);
  }

  void EditorRenderer::Create() {
    glClearColor(0.0, 0.0, 0.0, 1.0);

    vertex_shader.Create(GL_VERTEX_SHADER, {kVertexShaderSource});
    point_geometry_shader.Create(GL_GEOMETRY_SHADER, {kPointGeometryShaderSource});
    edge_geometry_shader.Create(GL_GEOMETRY_SHADER, {kEdgeGeometryShaderSource});
    fragment_shader.Create(GL_FRAGMENT_SHADER, {kFragmentShaderSource});

    face_program.Create({&vertex_shader, &fragment_shader});
    face_program.CompileAndLink();

    edge_program.Create({&vertex_shader, &edge_geometry_shader, &fragment_shader});
    edge_program.CompileAndLink();

    point_program.Create({&vertex_shader, &point_geometry_shader, &fragment_shader});
    point_program.CompileAndLink();

    vertex_format.Create({
      {u8"vertex_position", GL_FLOAT, 2},
      {u8"vertex_color", GL_FLOAT, 4}
    });

    pathfinding_edges_buffer.Create(GL_ARRAY_BUFFER);
    pathfinding_edges_array.Create();
    vertex_format.Apply(pathfinding_edges_array, edge_program);
    CHECK_STATE(!glGetError());

    pathfinding_nodes_buffer.Create(GL_ARRAY_BUFFER);
    pathfinding_nodes_array.Create();
    vertex_format.Apply(pathfinding_nodes_array, point_program);
    CHECK_STATE(!glGetError());

    selected_face_buffer.Create(GL_ARRAY_BUFFER);
    selected_face_array.Create();
    vertex_format.Apply(selected_face_array, face_program);
    CHECK_STATE(!glGetError());

    selected_interior_edge_buffer.Create(GL_ARRAY_BUFFER);
    selected_interior_edge_array.Create();
    vertex_format.Apply(selected_interior_edge_array, edge_program);
    CHECK_STATE(!glGetError());

    selected_exterior_edge_buffer.Create(GL_ARRAY_BUFFER);
    selected_exterior_edge_array.Create();
    vertex_format.Apply(selected_exterior_edge_array, edge_program);
    CHECK_STATE(!glGetError());

    selected_point_buffer.Create(GL_ARRAY_BUFFER);
    selected_point_array.Create();
    vertex_format.Apply(selected_point_array, point_program);
    CHECK_STATE(!glGetError());

    move_indicator_buffer.Create(GL_ARRAY_BUFFER);
    move_indicator_array.Create();
    vertex_format.Apply(move_indicator_array, edge_program);
    CHECK_STATE(!glGetError());

    selection_box_buffer.Create(GL_ARRAY_BUFFER);
    selection_box_array.Create();
    vertex_format.Apply(selection_box_array, edge_program);
    CHECK_STATE(!glGetError());

    mesh_renderer.Create();
  }

  void EditorRenderer::Render() {
    Drawable pathfinding_edges_data = editor.PathfindingEdges();
    pathfinding_edges_buffer.Data(pathfinding_edges_data.data_size(),
                 pathfinding_edges_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable pathfinding_nodes_data = editor.PathfindingNodes();
    pathfinding_nodes_buffer.Data(pathfinding_nodes_data.data_size(),
                 pathfinding_nodes_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_face_data = editor.HighlightedTriangles();
    selected_face_buffer.Data(selected_face_data.data_size(),
                 selected_face_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_interior_edge_data = editor.HighlightedWireframe();
    selected_interior_edge_buffer.Data(selected_interior_edge_data.data_size(),
                 selected_interior_edge_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_exterior_edge_data = editor.HighlightedWireframeExterior();
    selected_exterior_edge_buffer.Data(selected_exterior_edge_data.data_size(),
                 selected_exterior_edge_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_point_data = editor.HighlightedPoints();
    selected_point_buffer.Data(selected_point_data.data_size(),
                 selected_point_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable move_indicator_data = editor.MoveScaleIndicator();
    move_indicator_buffer.Data(move_indicator_data.data_size(),
                 move_indicator_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selection_box_data = editor.SelectionBox();
    selection_box_buffer.Data(selection_box_data.data_size(),
                 selection_box_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mesh_renderer.Render();

//    edge_program.Use();
//    edge_program.Uniforms({
//      {u8"projection", projection},
//      {u8"model_view", model_view}
//    });
//    edge_program.Uniforms({
//      {u8"line_width", 0.01},
//      {u8"inverse_aspect_ratio", inverse_aspect_ratio}
//    });
//    pathfinding_edges_array.Bind();
//    glDrawArrays(pathfinding_edges_data.element_type, 0, pathfinding_edges_data.element_count);
//    CHECK_STATE(!glGetError());
//
//    point_program.Use();
//    point_program.Uniforms({
//      {u8"projection", projection},
//      {u8"model_view", model_view}
//    });
//    point_program.Uniforms({
//      {u8"point_size", 0.02},
//      {u8"inverse_aspect_ratio", inverse_aspect_ratio}
//    });
//    pathfinding_nodes_array.Bind();
//    glDrawArrays(pathfinding_nodes_data.element_type, 0, pathfinding_nodes_data.element_count);
//    CHECK_STATE(!glGetError());

    edge_program.Use();
    edge_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    edge_program.Uniforms({
      {u8"line_width", 0.005},
      {u8"inverse_aspect_ratio", inverse_aspect_ratio}
    });
    move_indicator_array.Bind();
    glDrawArrays(move_indicator_data.element_type, 0, move_indicator_data.element_count);
    CHECK_STATE(!glGetError());

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    selected_face_array.Bind();
    glDrawArrays(selected_face_data.element_type, 0, selected_face_data.element_count);
    CHECK_STATE(!glGetError());

    edge_program.Use();
    edge_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    edge_program.Uniforms({
      {u8"line_width", 0.00125},
      {u8"inverse_aspect_ratio", inverse_aspect_ratio}
    });
    selected_interior_edge_array.Bind();
    glDrawArrays(selected_interior_edge_data.element_type,
                 0, selected_interior_edge_data.element_count);
    CHECK_STATE(!glGetError());

    edge_program.Use();
    edge_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    edge_program.Uniforms({
      {u8"line_width", 0.005},
      {u8"inverse_aspect_ratio", inverse_aspect_ratio}
    });
    selected_exterior_edge_array.Bind();
    glDrawArrays(selected_exterior_edge_data.element_type,
                 0, selected_exterior_edge_data.element_count);
    CHECK_STATE(!glGetError());

    point_program.Use();
    point_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    point_program.Uniforms({
      {u8"point_size", 0.01},
      {u8"inverse_aspect_ratio", inverse_aspect_ratio}
    });
    selected_point_array.Bind();
    glDrawArrays(selected_point_data.element_type, 0, selected_point_data.element_count);
    CHECK_STATE(!glGetError());

    edge_program.Use();
    edge_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    edge_program.Uniforms({
      {u8"line_width", 0.005},
      {u8"inverse_aspect_ratio", inverse_aspect_ratio}
    });
    selection_box_array.Bind();
    glDrawArrays(selection_box_data.element_type, 0, selection_box_data.element_count);
    CHECK_STATE(!glGetError());
  }

}  // namespace textengine
