#ifndef TEXTENGINE_MESH_H_
#define TEXTENGINE_MESH_H_

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "drawable.h"

namespace textengine {

  class Mesh {
  public:
    Mesh() = default;

    virtual ~Mesh() = default;

    struct HalfEdge;

    struct Face {
      HalfEdge *face_edge;
    };

    struct Vertex {
      HalfEdge *vertex_edge;
      glm::vec2 position;
    };

    struct HalfEdge {
      Face *face;
      HalfEdge *next, *opposite, *previous;
      Vertex *start;
    };

    const std::vector<std::unique_ptr<HalfEdge>> &get_half_edges() const;

    const std::vector<std::unique_ptr<Face>> &get_faces() const;

    const std::vector<std::unique_ptr<Vertex>> &get_vertices() const;

    void AddDefaultFace(glm::vec2 position);

    Vertex *ExtrudeEdge(HalfEdge *edge);

    Drawable Points() const;

    Drawable Triangulate() const;

    Drawable Wireframe() const;

  private:
    std::vector<std::unique_ptr<HalfEdge>> half_edges;
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<Vertex>> vertices;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESH_H_
