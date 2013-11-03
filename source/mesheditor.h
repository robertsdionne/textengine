#ifndef TEXTENGINE_MESHEDITOR_H_
#define TEXTENGINE_MESHEDITOR_H_

#include <glm/glm.hpp>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include "controller.h"

namespace textengine {

  class Keyboard;
  class Mesh;
  class Mesh::HalfEdge;
  class Mesh::Vertex;
  class Mouse;

  class MeshEditor : public Controller {
  public:
    MeshEditor(int width, int height, Keyboard &keyboard, Mouse &mouse, Mesh &mesh,
               std::default_random_engine &engine, const std::string &filename);

    virtual ~MeshEditor() = default;

    glm::vec2 get_cursor_position() const;

    std::unordered_set<Mesh::Vertex *> potentially_selected_vertices() const;

    std::unordered_set<Mesh::HalfEdge *> potentially_selected_half_edges() const;

    std::unordered_set<Mesh::Face *> potentially_selected_faces() const;

    std::unordered_set<Mesh::HalfEdge *> selected_half_edges() const;

    std::unordered_set<Mesh::Face *> selected_faces() const;

    void set_model_view_projection(glm::mat4 model_view_projection);

    Drawable HighlightedPoints() const;

    Drawable HighlightedTriangles() const;

    Drawable HighlightedWireframe() const;

    Drawable HighlightedWireframeExterior() const;

    Drawable MoveScaleIndicator() const;

    Drawable PathfindingEdges() const;

    Drawable PathfindingNodes() const;

    Drawable SelectionBox() const;

    virtual void Update() override;

  private:
    Mesh::RoomInfo *CreateRandomizedRoomInfo();

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
    std::unordered_set<Mesh::Vertex *> selected_vertices, additionally_selected_vertices;
    std::unordered_map<Mesh::Vertex *, glm::vec2> selected_vertex_positions;
    glm::vec2 cursor_start_position, center_of_mass;
    glm::mat4 model_view_projection;
    bool add_selecting, rotating, selecting;
    MoveMode moving;
    ScaleMode scaling;
    std::default_random_engine &engine;
    std::string filename;
    std::uniform_real_distribution<> uniform_real;
    std::uniform_int_distribution<char> uniform_int;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESHEDITOR_H_
