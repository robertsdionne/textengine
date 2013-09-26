#include <glm/glm.hpp>

#include "keyboard.h"
#include "mesh.h"
#include "mesheditor.h"

namespace textengine {

  MeshEditor::MeshEditor(Keyboard &keyboard, Mouse &mouse, Mesh &mesh)
  : keyboard(keyboard), mouse(mouse), mesh(mesh),
    selected_vertex0(mesh.get_vertices()[0].get()),
    selected_vertex1(mesh.get_vertices()[0]->vertex_edge->next->start),
    selected_edge(mesh.get_vertices()[0]->vertex_edge) {}

  std::unique_ptr<float[]> MeshEditor::HighlightedPoints() const {
    constexpr size_t kCoordinatesPerVertex = 2;
    std::unique_ptr<float[]> data{new float[kCoordinatesPerVertex * 2]};
    data[0] = selected_vertex0->position.x;
    data[1] = selected_vertex0->position.y;
    data[2] = selected_vertex1->position.x;
    data[3] = selected_vertex1->position.y;
    return data;
  }

  std::unique_ptr<float[]> MeshEditor::HighlightedWireframe() const {
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kEdgeSize = kVerticesPerEdge * kCoordinatesPerVertex;
    std::unique_ptr<float[]> data{new float[kEdgeSize]};
    data[0] = selected_edge->start->position.x;
    data[1] = selected_edge->start->position.y;
    data[2] = selected_edge->next->start->position.x;
    data[3] = selected_edge->next->start->position.y;
    return data;
  }

  void MeshEditor::Update() {
    
  }

}  // namespace textengine
