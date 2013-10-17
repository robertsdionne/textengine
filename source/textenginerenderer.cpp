#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "checks.h"
#include "gamestate.h"
#include "mesh.h"
#include "mesheditor.h"
#include "textenginerenderer.h"
#include "updater.h"

namespace textengine {

  constexpr const char *TextEngineRenderer::kVertexShaderSource;
  constexpr const char *TextEngineRenderer::kPointGeometryShaderSource;
  constexpr const char *TextEngineRenderer::kEdgeGeometryShaderSource;
  constexpr const char *TextEngineRenderer::kFragmentShaderSource;

  TextEngineRenderer::TextEngineRenderer(Updater &updater, Mesh &mesh, MeshEditor &editor)
  : updater(updater), mesh(mesh), editor(editor), model_view(glm::mat4()),
  projection(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)) {}

  void TextEngineRenderer::Change(int width, int height) {
    glViewport(0, 0, width, height);
    inverse_aspect_ratio = static_cast<float>(height) / static_cast<float>(width);
    projection = glm::ortho(-1.0f, 1.0f, -inverse_aspect_ratio, inverse_aspect_ratio, -1.0f, 1.0f);
    editor.set_model_view_projection(projection * model_view);
  }

  void TextEngineRenderer::Create() {
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

    float player_data[] = {
      3.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    float player_edge_data[] = {
      3.0f, 0.0f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, 1.0f, 0.25f, 0.25f, 0.25f, 1.0f,
      3.0f, 0.0f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, -1.0f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, 1.0f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, -1.0f, 0.25f, 0.25f, 0.25f, 1.0f
    };

    float npc_data[] = {
      3.0f, 0.0f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, 1.0f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, -1.0f, 0.25f, 0.25f, 0.25f, 1.0f
    };
    float npc_edge_data[] = {
      3.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      3.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };

    vertex_format.Create({
      {u8"vertex_position", GL_FLOAT, 2},
      {u8"vertex_color", GL_FLOAT, 4}
    });

    player_buffer.Create(GL_ARRAY_BUFFER);
    player_buffer.Data(sizeof(player_data), player_data, GL_STATIC_DRAW);
    player_array.Create();
    vertex_format.Apply(player_array, face_program);
    CHECK_STATE(!glGetError());

    player_edge_buffer.Create(GL_ARRAY_BUFFER);
    player_edge_buffer.Data(sizeof(player_edge_data), player_edge_data, GL_STATIC_DRAW);
    player_edge_array.Create();
    vertex_format.Apply(player_edge_array, edge_program);
    CHECK_STATE(!glGetError());

    mesh_buffer.Create(GL_ARRAY_BUFFER);
    mesh_array.Create();
    vertex_format.Apply(mesh_array, face_program);
    CHECK_STATE(!glGetError());

    mesh_edge_buffer.Create(GL_ARRAY_BUFFER);
    mesh_edge_array.Create();
    vertex_format.Apply(mesh_edge_array, edge_program);
    CHECK_STATE(!glGetError());

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

    npc_buffer.Create(GL_ARRAY_BUFFER);
    npc_buffer.Data(sizeof(npc_data), npc_data, GL_STATIC_DRAW);
    npc_array.Create();
    vertex_format.Apply(npc_array, face_program);
    CHECK_STATE(!glGetError());

    npc_edge_buffer.Create(GL_ARRAY_BUFFER);
    npc_edge_buffer.Data(sizeof(npc_edge_data), npc_edge_data, GL_STATIC_DRAW);
    npc_edge_array.Create();
    vertex_format.Apply(npc_edge_array, edge_program);
    CHECK_STATE(!glGetError());
  }

  void TextEngineRenderer::Render() {
    Drawable face_data = mesh.Triangulate();
    mesh_buffer.Data(sizeof(float) * face_data.data_size, face_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable edge_data = mesh.Wireframe();
    mesh_edge_buffer.Data(sizeof(float) * edge_data.data_size,
                          edge_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable pathfinding_edges_data = editor.PathfindingEdges();
    pathfinding_edges_buffer.Data(sizeof(float) * pathfinding_edges_data.data_size,
                 pathfinding_edges_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable pathfinding_nodes_data = editor.PathfindingNodes();
    pathfinding_nodes_buffer.Data(sizeof(float) * pathfinding_nodes_data.data_size,
                 pathfinding_nodes_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_face_data = editor.HighlightedTriangles();
    selected_face_buffer.Data(sizeof(float) * selected_face_data.data_size,
                 selected_face_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_interior_edge_data = editor.HighlightedWireframe();
    selected_interior_edge_buffer.Data(sizeof(float) * selected_interior_edge_data.data_size,
                 selected_interior_edge_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_exterior_edge_data = editor.HighlightedWireframeExterior();
    selected_exterior_edge_buffer.Data(sizeof(float) * selected_exterior_edge_data.data_size,
                 selected_exterior_edge_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_point_data = editor.HighlightedPoints();
    selected_point_buffer.Data(sizeof(float) * selected_point_data.data_size,
                 selected_point_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable move_indicator_data = editor.MoveScaleIndicator();
    move_indicator_buffer.Data(sizeof(float) * move_indicator_data.data_size,
                 move_indicator_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selection_box_data = editor.SelectionBox();
    selection_box_buffer.Data(sizeof(float) * selection_box_data.data_size,
                 selection_box_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GameState current_state = updater.GetCurrentState();

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    mesh_array.Bind();
    glDrawArrays(face_data.element_type, 0, face_data.element_count);
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
    mesh_edge_array.Bind();
    glDrawArrays(edge_data.element_type, 0, edge_data.element_count);
    CHECK_STATE(!glGetError());

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

    editor.Update();

    const float angle = glm::atan(current_state.player.direction.y, current_state.player.direction.x);
    const glm::mat4 player_model_view = model_view * (glm::translate(glm::mat4(), glm::vec3(current_state.player.position, 0.0)) *
                                                      glm::rotate(glm::mat4(), glm::degrees(angle), glm::vec3(0, 0, 1)) *
                                                      glm::scale(glm::mat4(), glm::vec3(0.01)));

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &player_model_view}
    });
    player_array.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 1*3);
    CHECK_STATE(!glGetError());

    edge_program.Use();
    edge_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &player_model_view}
    });
    edge_program.Uniforms({
      {u8"line_width", 0.00125},
      {u8"inverse_aspect_ratio", inverse_aspect_ratio}
    });
    player_edge_array.Bind();
    glDrawArrays(GL_LINES, 0, 6);
    CHECK_STATE(!glGetError());

    for (auto &character : current_state.non_player_characters) {
      const float angle = glm::atan(character.character.direction.y, character.character.direction.x);
      const glm::mat4 player_model_view = model_view * (glm::translate(glm::mat4(), glm::vec3(character.character.position, 0.0)) *
                                                        glm::rotate(glm::mat4(), glm::degrees(angle), glm::vec3(0, 0, 1)) *
                                                        glm::scale(glm::mat4(), glm::vec3(0.01)));

      face_program.Use();
      face_program.Uniforms({
        {u8"projection", &projection},
        {u8"model_view", &player_model_view}
      });
      npc_array.Bind();
      glDrawArrays(GL_TRIANGLES, 0, 1*3);
      CHECK_STATE(!glGetError());

      edge_program.Use();
      edge_program.Uniforms({
        {u8"projection", &projection},
        {u8"model_view", &player_model_view}
      });
      edge_program.Uniforms({
        {u8"line_width", 0.00125},
        {u8"inverse_aspect_ratio", inverse_aspect_ratio}
      });
      npc_edge_array.Bind();
      glDrawArrays(GL_LINES, 0, 6);
      CHECK_STATE(!glGetError());
    }
  }

}  // namespace textengine
