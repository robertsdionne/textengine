#ifndef TEXTENGINE_MESHEDITOR_H_
#define TEXTENGINE_MESHEDITOR_H_

#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>

namespace textengine {

  class Keyboard;
  class Mesh;
  class Mesh::HalfEdge;
  class Mesh::Vertex;
  class Mouse;

  class MeshEditor {
  public:
    MeshEditor(int width, int height, Keyboard &keyboard, Mouse &mouse, Mesh &mesh);

    virtual ~MeshEditor() = default;

    glm::vec2 get_cursor_position() const;

    std::unordered_set<Mesh::HalfEdge *> selected_half_edges() const;

    std::unordered_set<Mesh::Face *> selected_faces() const;

    Drawable HighlightedPoints() const;

    Drawable HighlightedTriangles() const;

    Drawable HighlightedWireframe() const;

    Drawable HighlightedWireframeExterior() const;

    Drawable MoveScaleIndicator() const;

    Drawable PathfindingEdges() const;

    Drawable PathfindingNodes() const;

    Drawable SelectionBox() const;

    void Update();

  private:
    glm::vec2 FaceCentroid(const Mesh::Face *face) const;

    enum class ScaleMode {
      kFalse = 0,
      kAll,
      kBoth,
      kX,
      kY
    };

    enum class MoveMode {
      kFalse = 0,
      kBoth,
      kX,
      kY
    };

    int width, height;
    Keyboard &keyboard;
    Mouse &mouse;
    Mesh &mesh;
    std::unordered_set<Mesh::Vertex *> selected_vertices;
    std::unordered_map<Mesh::Vertex *, glm::vec2> selected_vertex_positions;
    glm::vec2 cursor_start_position, center_of_mass;
    bool rotating, selecting;
    MoveMode moving;
    ScaleMode scaling;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESHEDITOR_H_
