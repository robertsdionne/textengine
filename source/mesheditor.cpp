#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_set>

#include "checks.h"
#include "drawable.h"
#include "keyboard.h"
#include "mesh.h"
#include "mesheditor.h"
#include "mouse.h"

namespace textengine {

  MeshEditor::MeshEditor(Keyboard &keyboard, Mouse &mouse, Mesh &mesh)
  : keyboard(keyboard), mouse(mouse), mesh(mesh), selected_vertices(),
    selected_vertex_positions(), cursor_start_position() {}

  glm::vec2 MeshEditor::get_cursor_position() const {
    return mouse.get_cursor_position() / glm::vec2(640, -480) + glm::vec2(0.0f, 1.0f);
  }

  std::unordered_set<Mesh::HalfEdge *> MeshEditor::selected_half_edges() const {
    std::unordered_set<Mesh::HalfEdge *> half_edges;
    for (const auto &half_edge : mesh.get_half_edges()) {
      if (selected_vertices.end() != selected_vertices.find(half_edge->start) &&
          selected_vertices.end() != selected_vertices.find(half_edge->next->start)) {
        half_edges.insert(half_edge.get());
      }
    }
    return half_edges;
  }

  std::unordered_set<Mesh::Face *> MeshEditor::selected_faces() const {
    std::unordered_set<Mesh::HalfEdge *> half_edges = selected_half_edges();
    std::unordered_set<Mesh::Face *> faces;
    for (const auto &face : mesh.get_faces()) {
      const auto h01 = face->face_edge;
      const auto h12 = h01->next;
      const auto h20 = h12->next;
      CHECK_STATE(h01 == h20->next);
      if (half_edges.end() != half_edges.find(h01) &&
          half_edges.end() != half_edges.find(h12) &&
          half_edges.end() != half_edges.find(h20)) {
        faces.insert(face.get());
      }
    }
    return faces;
  }

  Drawable MeshEditor::HighlightedPoints() const {
    Drawable drawable;
    constexpr size_t kCoordinatesPerVertex = 2;
    drawable.data_size = kCoordinatesPerVertex * selected_vertices.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    auto vertex = selected_vertices.begin();
    for (auto i = 0; i < selected_vertices.size(); ++i, ++vertex) {
      drawable.data[kCoordinatesPerVertex * i + 0] = (*vertex)->position.x;
      drawable.data[kCoordinatesPerVertex * i + 1] = (*vertex)->position.y;
    }
    drawable.element_count = static_cast<GLsizei>(selected_vertices.size());
    drawable.element_type = GL_POINTS;
    return drawable;
  }

  Drawable MeshEditor::HighlightedTriangles() const {
    std::unordered_set<Mesh::Face *> faces = selected_faces();
    Drawable drawable;
    constexpr size_t kVerticesPerFace = 3;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kFaceSize = kVerticesPerFace * kCoordinatesPerVertex;
    drawable.data_size = kFaceSize * faces.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    auto face = faces.begin();
    for (auto i = 0; i < faces.size(); ++i, ++face) {
      const auto h01 = (*face)->face_edge;
      const auto h12 = h01->next;
      const auto h20 = h12->next;
      CHECK_STATE(h01 == h20->next);
      const auto v0 = h01->start, v1 = h12->start, v2 = h20->start;
      drawable.data[kFaceSize * i + 0] = v0->position.x;
      drawable.data[kFaceSize * i + 1] = v0->position.y;
      drawable.data[kFaceSize * i + 2] = v1->position.x;
      drawable.data[kFaceSize * i + 3] = v1->position.y;
      drawable.data[kFaceSize * i + 4] = v2->position.x;
      drawable.data[kFaceSize * i + 5] = v2->position.y;
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerFace * faces.size());
    drawable.element_type = GL_TRIANGLES;
    return drawable;
  }

  Drawable MeshEditor::HighlightedWireframe() const {
    std::unordered_set<Mesh::HalfEdge *> half_edges = selected_half_edges();
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kEdgeSize = kVerticesPerEdge * kCoordinatesPerVertex;
    drawable.data_size = kEdgeSize * half_edges.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    auto half_edge = half_edges.begin();
    for (auto i = 0; i < half_edges.size(); ++i, ++half_edge) {
      drawable.data[kEdgeSize * i + 0] = (*half_edge)->start->position.x;
      drawable.data[kEdgeSize * i + 1] = (*half_edge)->start->position.y;
      drawable.data[kEdgeSize * i + 2] = (*half_edge)->next->start->position.x;
      drawable.data[kEdgeSize * i + 3] = (*half_edge)->next->start->position.y;
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerEdge * half_edges.size());
    drawable.element_type = GL_LINES;
    return drawable;
  }

  void MeshEditor::Update() {
    const bool ready = !(selecting || moving);
    if (ready && keyboard.IsKeyJustPressed('A')) {
      if (selected_vertices.empty()) {
        for (auto &vertex : mesh.get_vertices()) {
          selected_vertices.insert(vertex.get());
        }
      } else {
        selected_vertices.clear();
      }
    }
    if (ready && mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      selected_vertices.clear();
      selecting = true;
      cursor_start_position = get_cursor_position();
    }
    if (ready && keyboard.IsKeyJustPressed('G')) {
      moving = true;
      for (auto vertex : selected_vertices) {
        selected_vertex_positions[vertex] = vertex->position;
      }
      cursor_start_position = get_cursor_position();
    }
    if (selecting && mouse.HasCursorMoved()) {
      selected_vertices.clear();
      const glm::vec2 cursor_end_position = get_cursor_position();
      const glm::vec2 top_left = glm::min(cursor_start_position, cursor_end_position);
      const glm::vec2 bottom_right = glm::max(cursor_start_position, cursor_end_position);
      for (auto &vertex : mesh.get_vertices()) {
        if (glm::all(glm::lessThan(top_left, vertex->position)) &&
            glm::all(glm::lessThan(vertex->position, bottom_right))) {
          selected_vertices.insert(vertex.get());
        }
      }
    }
    if (selecting && keyboard.IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
      selecting = false;
      selected_vertices.clear();
    }
    if (selecting && mouse.IsButtonJustReleased(GLFW_MOUSE_BUTTON_1)) {
      selecting = false;
    }
    if (moving && mouse.HasCursorMoved()) {
      const glm::vec2 d = get_cursor_position() - cursor_start_position;
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex] + d;
      }
    }
    if (moving && keyboard.IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
      moving = false;
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex];
      }
    }
    if (moving && mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      moving = false;
      selected_vertex_positions.clear();
    }
  }

}  // namespace textengine
