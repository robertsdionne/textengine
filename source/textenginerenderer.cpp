#include <GLFW/glfw3.h>
#include <memory>

#include "checks.h"
#include "drawtools.h"
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
  : updater(updater), mesh(mesh), editor(editor) {}

  void TextEngineRenderer::Change(int width, int height) {
    glViewport(0, 0, width, height);
  }

  void TextEngineRenderer::Create() {
    glClearColor(0.0, 0.0, 0.0, 1.0);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &kVertexShaderSource, nullptr);
    GLint compile_status;
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    char info_log[4 * 1024];
    if (!compile_status) {
      glGetShaderInfoLog(vertex_shader, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    point_geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(point_geometry_shader, 1, &kPointGeometryShaderSource, nullptr);
    glCompileShader(point_geometry_shader);
    glGetShaderiv(point_geometry_shader, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
      glGetShaderInfoLog(point_geometry_shader, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    edge_geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(edge_geometry_shader, 1, &kEdgeGeometryShaderSource, nullptr);
    glCompileShader(edge_geometry_shader);
    glGetShaderiv(edge_geometry_shader, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
      glGetShaderInfoLog(edge_geometry_shader, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &kFragmentShaderSource, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
      glGetShaderInfoLog(fragment_shader, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    face_program = glCreateProgram();
    glAttachShader(face_program, vertex_shader);
    glAttachShader(face_program, fragment_shader);
    glLinkProgram(face_program);
    GLint link_status;
    glGetProgramiv(face_program, GL_LINK_STATUS, &link_status);
    if (!link_status) {
      glGetProgramInfoLog(face_program, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    edge_program = glCreateProgram();
    glAttachShader(edge_program, vertex_shader);
    glAttachShader(edge_program, edge_geometry_shader);
    glAttachShader(edge_program, fragment_shader);
    glLinkProgram(edge_program);
    glGetProgramiv(edge_program, GL_LINK_STATUS, &link_status);
    if (!link_status) {
      glGetProgramInfoLog(edge_program, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    point_program = glCreateProgram();
    glAttachShader(point_program, vertex_shader);
    glAttachShader(point_program, point_geometry_shader);
    glAttachShader(point_program, fragment_shader);
    glLinkProgram(point_program);
    glGetProgramiv(point_program, GL_LINK_STATUS, &link_status);
    if (!link_status) {
      glGetProgramInfoLog(point_program, sizeof(info_log), nullptr, info_log);
      FAIL(info_log);
    }
    CHECK_STATE(!glGetError());

    constexpr int kCircleResolution = 100;
    float circle_data[kCircleResolution * 6];
    for (int i = 0; i < kCircleResolution; ++i) {
      const float theta_0 = 2.0 * M_PI * i / kCircleResolution;
      const float theta_1 = 2.0 * M_PI * (i + 1) / kCircleResolution;
      circle_data[6 * i] = cosf(theta_0);
      circle_data[6 * i + 1] = sinf(theta_0);
      circle_data[6 * i + 2] = cosf(theta_1);
      circle_data[6 * i + 3] = sinf(theta_1);
      circle_data[6 * i + 4] = 0.0f;
      circle_data[6 * i + 5] = 0.0f;
    }

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_data), circle_data, GL_STATIC_DRAW);
    CHECK_STATE(!glGetError());

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(face_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(face_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());
    
    glGenBuffers(1, &face_vertex_buffer);

    glGenVertexArrays(1, &face_vertex_array);
    glBindVertexArray(face_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, face_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(face_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(face_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &edge_vertex_buffer);

    glGenVertexArrays(1, &edge_vertex_array);
    glBindVertexArray(edge_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, edge_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(edge_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(edge_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &pathfinding_edges_vertex_buffer);

    glGenVertexArrays(1, &pathfinding_edges_vertex_array);
    glBindVertexArray(pathfinding_edges_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, pathfinding_edges_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(edge_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(edge_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &pathfinding_nodes_vertex_buffer);

    glGenVertexArrays(1, &pathfinding_nodes_vertex_array);
    glBindVertexArray(pathfinding_nodes_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, pathfinding_nodes_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(point_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(point_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &selected_face_vertex_buffer);

    glGenVertexArrays(1, &selected_face_vertex_array);
    glBindVertexArray(selected_face_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, selected_face_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(face_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(face_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &selected_interior_edge_vertex_buffer);

    glGenVertexArrays(1, &selected_interior_edge_vertex_array);
    glBindVertexArray(selected_interior_edge_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, selected_interior_edge_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(edge_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(edge_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &selected_exterior_edge_vertex_buffer);

    glGenVertexArrays(1, &selected_exterior_edge_vertex_array);
    glBindVertexArray(selected_exterior_edge_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, selected_exterior_edge_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(edge_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(edge_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &selected_point_vertex_buffer);

    glGenVertexArrays(1, &selected_point_vertex_array);
    glBindVertexArray(selected_point_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, selected_point_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(point_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(point_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &move_indicator_vertex_buffer);

    glGenVertexArrays(1, &move_indicator_vertex_array);
    glBindVertexArray(move_indicator_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, move_indicator_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(edge_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(edge_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());

    glGenBuffers(1, &selection_box_vertex_buffer);

    glGenVertexArrays(1, &selection_box_vertex_array);
    glBindVertexArray(selection_box_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, selection_box_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(edge_program, u8"vertex_position"),
                          2, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(glGetAttribLocation(edge_program, u8"vertex_position"));
    CHECK_STATE(!glGetError());
  }

  void TextEngineRenderer::Render() {
    Drawable face_data = mesh.Triangulate();
    
    glBindBuffer(GL_ARRAY_BUFFER, face_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * face_data.data_size, face_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable edge_data = mesh.Wireframe();

    glBindBuffer(GL_ARRAY_BUFFER, edge_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * edge_data.data_size, edge_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable pathfinding_edges_data = editor.PathfindingEdges();

    glBindBuffer(GL_ARRAY_BUFFER, pathfinding_edges_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pathfinding_edges_data.data_size,
                 pathfinding_edges_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable pathfinding_nodes_data = editor.PathfindingNodes();

    glBindBuffer(GL_ARRAY_BUFFER, pathfinding_nodes_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pathfinding_nodes_data.data_size,
                 pathfinding_nodes_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_face_data = editor.HighlightedTriangles();

    glBindBuffer(GL_ARRAY_BUFFER, selected_face_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * selected_face_data.data_size,
                 selected_face_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_interior_edge_data = editor.HighlightedWireframe();

    glBindBuffer(GL_ARRAY_BUFFER, selected_interior_edge_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * selected_interior_edge_data.data_size,
                 selected_interior_edge_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_exterior_edge_data = editor.HighlightedWireframeExterior();

    glBindBuffer(GL_ARRAY_BUFFER, selected_exterior_edge_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * selected_exterior_edge_data.data_size,
                 selected_exterior_edge_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selected_point_data = editor.HighlightedPoints();

    glBindBuffer(GL_ARRAY_BUFFER, selected_point_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * selected_point_data.data_size,
                 selected_point_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable move_indicator_data = editor.MoveIndicator();

    glBindBuffer(GL_ARRAY_BUFFER, move_indicator_vertex_array);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * move_indicator_data.data_size,
                 move_indicator_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable selection_box_data = editor.SelectionBox();

    glBindBuffer(GL_ARRAY_BUFFER, selection_box_vertex_array);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * selection_box_data.data_size,
                 selection_box_data.data.get(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GameState current_state = updater.GetCurrentState();
    
    glUseProgram(face_program);
    glUniform2f(glGetUniformLocation(face_program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(face_program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(face_program, u8"shape_color"), 0.640000, 0.640000, 0.640000, 1);
    glBindVertexArray(face_vertex_array);
    glDrawArrays(face_data.element_type, 0, face_data.element_count);
    CHECK_STATE(!glGetError());

    glUseProgram(edge_program);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(edge_program, u8"shape_color"),
                0.640000/2.0, 0.640000/2.0, 0.640000/2.0, 1);
    glUniform1f(glGetUniformLocation(edge_program, u8"line_width"), 0.0025);
    glBindVertexArray(edge_vertex_array);
    glDrawArrays(edge_data.element_type, 0, edge_data.element_count);
    CHECK_STATE(!glGetError());

//    glUseProgram(edge_program);
//    glUniform2f(glGetUniformLocation(edge_program, u8"shape_position"), -1, -1);
//    glUniform2f(glGetUniformLocation(edge_program, u8"shape_size"), 2, 2);
//    glUniform4f(glGetUniformLocation(edge_program, u8"shape_color"), 0.0, 0.0, 0.64, 1);
//    glUniform1f(glGetUniformLocation(edge_program, u8"line_width"), 0.01);
//    glBindVertexArray(pathfinding_edges_vertex_array);
//    glDrawArrays(pathfinding_edges_data.element_type, 0, pathfinding_edges_data.element_count);
//    CHECK_STATE(!glGetError());
//
//    glUseProgram(point_program);
//    glUniform2f(glGetUniformLocation(point_program, u8"shape_position"), -1, -1);
//    glUniform2f(glGetUniformLocation(point_program, u8"shape_size"), 2, 2);
//    glUniform4f(glGetUniformLocation(point_program, u8"shape_color"), 0.0, 0.0, 0.64/2.0, 1);
//    glUniform1f(glGetUniformLocation(point_program, u8"point_size"), 0.04);
//    glBindVertexArray(pathfinding_nodes_vertex_array);
//    glDrawArrays(pathfinding_nodes_data.element_type, 0, pathfinding_nodes_data.element_count);
//    CHECK_STATE(!glGetError());

    glUseProgram(edge_program);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(edge_program, u8"shape_color"), 0.64, 0.64, 0.0, 1);
    glUniform1f(glGetUniformLocation(edge_program, u8"line_width"), 0.01);
    glBindVertexArray(move_indicator_vertex_array);
    glDrawArrays(move_indicator_data.element_type, 0, move_indicator_data.element_count);
    CHECK_STATE(!glGetError());
    
    glUseProgram(face_program);
    glUniform2f(glGetUniformLocation(face_program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(face_program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(face_program, u8"shape_color"), 0.32, 0.0, 0.0, 1);
    glBindVertexArray(selected_face_vertex_array);
    glDrawArrays(selected_face_data.element_type, 0, selected_face_data.element_count);
    CHECK_STATE(!glGetError());

    glUseProgram(edge_program);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(edge_program, u8"shape_color"), 0.64, 0.0, 0.0, 1);
    glUniform1f(glGetUniformLocation(edge_program, u8"line_width"), 0.0025);
    glBindVertexArray(selected_interior_edge_vertex_array);
    glDrawArrays(selected_interior_edge_data.element_type,
                 0, selected_interior_edge_data.element_count);
    CHECK_STATE(!glGetError());

    glUseProgram(edge_program);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(edge_program, u8"shape_color"), 0.64, 0.0, 0.0, 1);
    glUniform1f(glGetUniformLocation(edge_program, u8"line_width"), 0.01);
    glBindVertexArray(selected_exterior_edge_vertex_array);
    glDrawArrays(selected_exterior_edge_data.element_type,
                 0, selected_exterior_edge_data.element_count);
    CHECK_STATE(!glGetError());

    glUseProgram(point_program);
    glUniform2f(glGetUniformLocation(point_program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(point_program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(point_program, u8"shape_color"), 1.0, 0.0, 0.0, 1);
    glUniform1f(glGetUniformLocation(point_program, u8"point_size"), 0.02);
    glBindVertexArray(selected_point_vertex_array);
    glDrawArrays(selected_point_data.element_type, 0, selected_point_data.element_count);
    CHECK_STATE(!glGetError());

    glUseProgram(edge_program);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_position"), -1, -1);
    glUniform2f(glGetUniformLocation(edge_program, u8"shape_size"), 2, 2);
    glUniform4f(glGetUniformLocation(edge_program, u8"shape_color"), 0.64, 0.64, 0.0, 1);
    glUniform1f(glGetUniformLocation(edge_program, u8"line_width"), 0.01);
    glBindVertexArray(selection_box_vertex_array);
    glDrawArrays(selection_box_data.element_type, 0, selection_box_data.element_count);
    CHECK_STATE(!glGetError());

    editor.Update();
    
//    glUseProgram(face_program);
//    glUniform2f(glGetUniformLocation(face_program, u8"shape_position"),
//                current_state.player_position.x, current_state.player_position.y);
//    glUniform2f(glGetUniformLocation(face_program, u8"shape_size"), 0.05, 0.05);
//    glUniform4f(glGetUniformLocation(face_program, u8"shape_color"), 1, 0, 0, 1);
//    glBindVertexArray(vertex_array);
//    glDrawArrays(GL_TRIANGLES, 0, 100*3);
//    CHECK_STATE(!glGetError());
//    glUseProgram(face_program);
//    glUniform2f(glGetUniformLocation(face_program, u8"shape_position"),
//                current_state.player_position.x + current_state.player_direction.x * 0.1,
//                current_state.player_position.y + current_state.player_direction.y * 0.1);
//    glUniform2f(glGetUniformLocation(face_program, u8"shape_size"), 0.01, 0.01);
//    glUniform4f(glGetUniformLocation(face_program, u8"shape_color"), 1, 0, 0, 1);
//    glBindVertexArray(vertex_array);
//    glDrawArrays(GL_TRIANGLES, 0, 100*3);
//    CHECK_STATE(!glGetError());
  }

}  // namespace textengine
