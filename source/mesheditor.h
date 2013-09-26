#ifndef TEXTENGINE_MESHEDITOR_H_
#define TEXTENGINE_MESHEDITOR_H_

namespace textengine {

  class Keyboard;
  class Mesh;
  class Mesh::HalfEdge;
  class Mesh::Vertex;

  class MeshEditor {
  public:
    MeshEditor(Keyboard &keyboard, Mesh &mesh);

    virtual ~MeshEditor() = default;

    std::unique_ptr<float[]> HighlightedPoints() const;

    std::unique_ptr<float[]> HighlightedWireframe() const;

    void Update();

  private:
    Keyboard &keyboard;
    Mesh &mesh;
    Mesh::Vertex *selected_vertex0, *selected_vertex1;
    Mesh::HalfEdge *selected_edge;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESHEDITOR_H_
