#ifndef TEXTENGINE_MESHEDITOR_H_
#define TEXTENGINE_MESHEDITOR_H_

#include <glm/glm.hpp>

namespace textengine {

  class Keyboard;
  class Mesh;
  class Mesh::HalfEdge;
  class Mesh::Vertex;
  class Mouse;

  class MeshEditor {
  public:
    MeshEditor(Keyboard &keyboard, Mouse &mouse, Mesh &mesh);

    virtual ~MeshEditor() = default;

    Drawable HighlightedPoints() const;

    Drawable HighlightedWireframe() const;

    void Update();

  private:
    Keyboard &keyboard;
    Mouse &mouse;
    Mesh &mesh;
    Mesh::Vertex *selected_vertex0, *selected_vertex1;
    Mesh::HalfEdge *selected_edge;
    glm::vec2 start_position, start_cursor_position;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESHEDITOR_H_
