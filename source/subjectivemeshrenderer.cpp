#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "checks.h"
#include "gamestate.h"
#include "mesh.h"
#include "mesheditor.h"
#include "shaders.h"
#include "subjectivemeshrenderer.h"
#include "updater.h"

namespace textengine {

  SubjectiveMeshRenderer::SubjectiveMeshRenderer(Mesh &mesh, Updater &updater)
  : mesh(mesh), model_view(glm::mat4()),
    projection(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)), perspective(), updater(updater) {}

  void SubjectiveMeshRenderer::Change(int width, int height) {
    inverse_aspect_ratio = static_cast<float>(height) / static_cast<float>(width);
    projection = glm::ortho(-1.0f, 1.0f, -inverse_aspect_ratio, inverse_aspect_ratio, -1.0f, 1.0f);
  }

  void SubjectiveMeshRenderer::Create() {
    vertex_shader.Create(GL_VERTEX_SHADER, {kVertexShaderSource});
    edge_geometry_shader.Create(GL_GEOMETRY_SHADER, {kEdgeGeometryShaderSource});
    fragment_shader.Create(GL_FRAGMENT_SHADER, {kFragmentShaderSource});

    face_program.Create({&vertex_shader, &fragment_shader});
    face_program.CompileAndLink();

    edge_program.Create({&vertex_shader, &edge_geometry_shader, &fragment_shader});
    edge_program.CompileAndLink();

    vertex_format.Create({
      {u8"vertex_position", GL_FLOAT, 2},
      {u8"vertex_color", GL_FLOAT, 4}
    });

    mesh_buffer.Create(GL_ARRAY_BUFFER);
    mesh_array.Create();
    vertex_format.Apply(mesh_array, face_program);
    CHECK_STATE(!glGetError());

    mesh_edge_buffer.Create(GL_ARRAY_BUFFER);
    mesh_edge_array.Create();
    vertex_format.Apply(mesh_edge_array, edge_program);
    CHECK_STATE(!glGetError());

    mesh_shadow_buffer.Create(GL_ARRAY_BUFFER);
    mesh_shadow_array.Create();
    vertex_format.Apply(mesh_shadow_array, face_program);
    CHECK_STATE(!glGetError());
  }

  void SubjectiveMeshRenderer::Render() {
    Drawable face_data = mesh.Triangulate(perspective, updater);
    mesh_buffer.Data(face_data.data_size(), face_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    mesh_array.Bind();
    glDrawArrays(face_data.element_type, 0, face_data.element_count);
    CHECK_STATE(!glGetError());
  }

  void SubjectiveMeshRenderer::RenderShadows() {
    Drawable shadow_data = mesh.Shadows(perspective);
    mesh_shadow_buffer.Data(shadow_data.data_size(), shadow_data.data.data(), GL_STREAM_DRAW);
    CHECK_STATE(!glGetError());

    face_program.Use();
    face_program.Uniforms({
      {u8"projection", &projection},
      {u8"model_view", &model_view}
    });
    mesh_shadow_array.Bind();
    glDrawArrays(shadow_data.element_type, 0, shadow_data.element_count);
    CHECK_STATE(!glGetError());
  }

  void SubjectiveMeshRenderer::RenderOutline() {
    Drawable edge_data = mesh.Wireframe(perspective);
    mesh_edge_buffer.Data(edge_data.data_size(), edge_data.data.data(), GL_STREAM_DRAW);
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
  }

  void SubjectiveMeshRenderer::SetPerspective(glm::vec2 perspective, glm::vec2 camera_position) {
    this->perspective = perspective;
    model_view = glm::scale(glm::mat4(), glm::vec3(1.0f)) * glm::translate(glm::mat4(), glm::vec3(-camera_position, 0.0f));
  }

}  // namespace textengine
