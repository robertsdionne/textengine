#ifndef TEXTENGINE_MESH_H_
#define TEXTENGINE_MESH_H_

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace textengine {

  class Mesh {
  public:
    Mesh() = default;

    virtual ~Mesh() = default;

    void AddDefaultFace(glm::vec2 position);

    std::unique_ptr<float[]> Points() const;

    std::unique_ptr<float[]> Triangulate() const;

    std::unique_ptr<float[]> Wireframe() const;

  private:
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

    std::vector<std::unique_ptr<HalfEdge>> half_edges;
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<Vertex>> vertices;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESH_H_
