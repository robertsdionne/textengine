#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gltext.hpp>
#include <memory>

#include "checks.h"
#include "gamestate.h"
#include "subjectivemeshrenderer.h"
#include "textenginerenderer.h"
#include "updater.h"

namespace textengine {

  TextEngineRenderer::TextEngineRenderer(Updater &updater, SubjectiveMeshRenderer &mesh_renderer)
  : updater(updater), mesh_renderer(mesh_renderer), model_view(glm::mat4()),
    projection(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)) {}

  void TextEngineRenderer::Change(int width, int height) {
    glViewport(0, 0, width, height);
    inverse_aspect_ratio = static_cast<float>(height) / static_cast<float>(width);
    projection = glm::ortho(-1.0f, 1.0f, -inverse_aspect_ratio, inverse_aspect_ratio, -1.0f, 1.0f);
    mesh_renderer.Change(width, height);
    font.setDisplaySize(width, height);
  }

  void TextEngineRenderer::Create() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
      2.0f, -2.0f, 0.0f, 0.0f, 0.7f, 1.0f,
      0.0f, 2.0f, 0.0f, 0.0f, 0.7f, 1.0f,
      0.0f, -2.0f, 0.0f, 0.0f, 0.7f, 1.0f,
      2.0f, -2.0f, 0.0f, 0.0f, 0.7f, 1.0f,
      2.0f, 2.0f, 0.0f, 0.0f, 0.7f, 1.0f,
      0.0f, 2.0f, 0.0f, 0.0f, 0.7f, 1.0f
    };
    float player_edge_data[] = {
      2.0f, -2.0f, 1.0f, 1.0f, 1.5f, 1.0f,
      2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, -2.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, -2.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      2.0f, -2.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };

    float npc_data[] = {
      1.5f, -1.5f, 0.7f, 0.7f, 0.7f, 1.0f,
      0.0f, 1.5f, 0.7f, 0.7f, 0.7f, 1.0f,
      0.0f, -1.5f, 0.7f, 0.7f, 0.7f, 1.0f,
      1.5f, -1.5f, 0.7f, 0.7f, 0.7f, 1.0f,
      1.5f, 1.5f, 0.7f, 0.7f, 0.7f, 1.0f,
      0.0f, 1.5f, 0.7f, 0.7f, 0.7f, 1.0f
    };
    float npc_edge_data[] = {
      1.5f, -1.5f, 0.25f, 0.25f, 0.25f, 1.0f,
      1.5f, 1.5f, 0.25f, 0.25f, 0.25f, 1.0f,
      1.5f, 1.5f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, 1.5f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, 1.5f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, -1.5f, 0.25f, 0.25f, 0.25f, 1.0f,
      0.0f, -1.5f, 0.25f, 0.25f, 0.25f, 1.0f,
      1.5f, -1.5f, 0.25f, 0.25f, 0.25f, 1.0f
    };

    float player_view_data[] = {
      0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
      1.0f, -2.0f, 1.0f, 1.0f, 1.0f, 0.0f,
      1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.0f
    };

    vertex_format.Create({
      {u8"vertex_position", GL_FLOAT, 2},
      {u8"vertex_color", GL_FLOAT, 4}
    });

    mesh_renderer.Create();

    player_view_buffer.Create(GL_ARRAY_BUFFER);
    player_view_buffer.Data(sizeof(player_view_data), player_view_data, GL_STATIC_DRAW);
    player_view_array.Create();
    vertex_format.Apply(player_view_array, face_program);
    CHECK_STATE(!glGetError());

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

    item_buffer.Create(GL_ARRAY_BUFFER);
    item_array.Create();
    vertex_format.Apply(item_array, face_program);
    CHECK_STATE(!glGetError());

    item_edge_buffer.Create(GL_ARRAY_BUFFER);
    item_edge_array.Create();
    vertex_format.Apply(item_edge_array, edge_program);
    CHECK_STATE(!glGetError());

    font = gltext::Font("../resource/ubuntu-font-family-0.80/Ubuntu-R.ttf", 32, 1024, 1024);
    font.cacheCharacters("1234567890!@#$%^&*()abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,./;'[]\\<>?:\"{}|-=_+");
  }

  void TextEngineRenderer::Render() {
    GameState &current_state = updater.GetCurrentState();

    Drawable item_data = current_state.TriangulateItems();
    item_buffer.Data(item_data.data_size(), item_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    Drawable item_edge_data = current_state.WireframeItems();
    item_edge_buffer.Data(item_edge_data.data_size(), item_edge_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec2 position = current_state.player.position;

    mesh_renderer.SetPerspective(position, current_state.camera_position);
    mesh_renderer.Render();

    model_view = glm::scale(glm::mat4(), glm::vec3(1.0f)) * glm::translate(glm::mat4(), glm::vec3(-current_state.camera_position, 0.0f));

    const float angle = glm::atan(current_state.player.direction.y, current_state.player.direction.x);
    const glm::mat4 player_model_view = model_view * (glm::translate(glm::mat4(), glm::vec3(position, 0.0f)) *
                                                      glm::rotate(glm::mat4(), glm::degrees(angle), glm::vec3(0, 0, 1)) *
                                                      glm::scale(glm::mat4(), glm::vec3(0.01f)));

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &player_model_view}
    });
    player_array.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
    glDrawArrays(GL_LINES, 0, 8);
    CHECK_STATE(!glGetError());

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    item_array.Bind();
    glDrawArrays(item_data.element_type, 0, item_data.element_count);
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
    item_edge_array.Bind();
    glDrawArrays(item_edge_data.element_type, 0, item_edge_data.element_count);
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
      glDrawArrays(GL_TRIANGLES, 0, 6);
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
      glDrawArrays(GL_LINES, 0, 8);
      CHECK_STATE(!glGetError());
    }

    mesh_renderer.RenderShadows();
  }

}  // namespace textengine
