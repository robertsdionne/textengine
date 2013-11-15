#ifndef __textengine__halfedgemesh__
#define __textengine__halfedgemesh__

#include <glm/glm.hpp>
#include <memory>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace textengine {

  class HalfEdge;
  class Vertex;

  struct Face {
    Face();

    bool IsDummy() const;

    HalfEdge *face_edge;
  };

  struct HalfEdge {
    HalfEdge();

    bool IsDummy() const;

    Face *face;
    HalfEdge *next, *opposite, *previous;
    Vertex *start;
  };

  struct Vertex {
    Vertex();
    Vertex(glm::vec2 position, HalfEdge *vertex_edge);

    glm::vec2 position;
    HalfEdge *vertex_edge;
  };

  class HalfEdgeMesh {
    template<typename T> using Set = std::unordered_set<std::unique_ptr<T>>;
    using CreationInfo = std::tuple<Face *,
        HalfEdge *, HalfEdge *, HalfEdge *, Vertex *, Vertex *, Vertex *>;

    HalfEdgeMesh();

  private:
    CreationInfo CreateFace(glm::vec2 point0, glm::vec2 point1, glm::vec2 point2,
                            HalfEdge *half_edge10 = nullptr,
                            HalfEdge *half_edge21 = nullptr,
                            HalfEdge *half_edge02 = nullptr);

    CreationInfo CreateFace(Vertex *vertex0, Vertex *vertex1, Vertex *vertex2,
                            HalfEdge *half_edge10 = nullptr,
                            HalfEdge *half_edge21 = nullptr,
                            HalfEdge *half_edge02 = nullptr);

  private:
    Set<Face> faces;
    Set<HalfEdge> half_edges;
    Set<Vertex> vertices;
    Vertex infinity;
  };

}  // namespace textengine

#endif /* defined(__textengine__halfedgemesh__) */
