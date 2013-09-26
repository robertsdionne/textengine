#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "drawable.h"
#include "keyboard.h"
#include "mesh.h"
#include "mesheditor.h"
#include "mouse.h"

namespace textengine {

  MeshEditor::MeshEditor(Keyboard &keyboard, Mouse &mouse, Mesh &mesh)
  : keyboard(keyboard), mouse(mouse), mesh(mesh),
    selected_vertex0(mesh.get_vertices()[0].get()),
    selected_vertex1(mesh.get_vertices()[0]->vertex_edge->next->start),
    selected_edge(mesh.get_vertices()[0]->vertex_edge) {}

  Drawable MeshEditor::HighlightedPoints() const {
    Drawable drawable;
    constexpr size_t kCoordinatesPerVertex = 2;
    drawable.data_size = kCoordinatesPerVertex * 2;
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    drawable.data[0] = selected_vertex0->position.x;
    drawable.data[1] = selected_vertex0->position.y;
    drawable.data[2] = selected_vertex1->position.x;
    drawable.data[3] = selected_vertex1->position.y;
    drawable.element_count = static_cast<GLsizei>(2);
    drawable.element_type = GL_POINTS;
    return drawable;
  }

  Drawable MeshEditor::HighlightedWireframe() const {
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kEdgeSize = kVerticesPerEdge * kCoordinatesPerVertex;
    drawable.data_size = kEdgeSize * 2;
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    drawable.data[0] = selected_edge->start->position.x;
    drawable.data[1] = selected_edge->start->position.y;
    drawable.data[2] = selected_edge->next->start->position.x;
    drawable.data[3] = selected_edge->next->start->position.y;
    drawable.element_count = static_cast<GLsizei>(kVerticesPerEdge);
    drawable.element_type = GL_LINES;
    return drawable;
  }

  void MeshEditor::Update() {
    if (keyboard.IsKeyJustPressed('E')) {
      if (selected_vertex0 && selected_vertex1) {
        selected_vertex0 = mesh.ExtrudeEdge(selected_edge);
        selected_edge = selected_vertex0->vertex_edge;
        selected_vertex1 = selected_edge->next->start;
      }
    }
    if (mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      if (selected_vertex0 && selected_vertex1) {
        start_position = selected_vertex0->position;
        start_cursor_position = mouse.get_cursor_position();
      }
    }
    if (mouse.IsButtonDown(GLFW_MOUSE_BUTTON_1) && mouse.HasMouseMoved()) {
      const glm::vec2 d = (mouse.get_cursor_position() - start_cursor_position) * glm::vec2(1, -1);
      selected_vertex0->position = start_position + d / 1000.0f;
    }
  }

}  // namespace textengine
