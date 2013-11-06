#ifndef TEXTENGINE_MESH_H_
#define TEXTENGINE_MESH_H_

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "drawable.h"

namespace textengine {

  class Mesh {
  public:
    struct HalfEdge;
    struct RoomInfo;

    struct Face {
      HalfEdge *face_edge;
      RoomInfo *room_info;
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

    struct RoomInfo {
      std::string name;
      glm::vec4 color;
    };

    Mesh() = default;

    Mesh(Mesh &&mesh) = default;

    Mesh(std::vector<std::unique_ptr<Face>> &&faces,
         std::vector<std::unique_ptr<HalfEdge>> &&half_edges,
         std::vector<std::unique_ptr<Vertex>> &&vertices,
         std::vector<std::unique_ptr<RoomInfo>> &&room_infos);

    virtual ~Mesh() = default;

    Mesh &operator =(Mesh &&mesh) = default;

    std::vector<std::unique_ptr<Face>> &get_faces();

    std::vector<std::unique_ptr<HalfEdge>> &get_half_edges();

    std::vector<std::unique_ptr<Vertex>> &get_vertices();

    std::vector<std::unique_ptr<RoomInfo>> &get_room_infos();

    void AddDefaultFace(glm::vec2 position);

    void AddFace(Vertex *vertex0, Vertex *vertex1, Vertex *vertex2);

    Vertex *ExtrudeEdge(HalfEdge *edge);

    std::vector<std::unique_ptr<std::vector<glm::vec2>>> Boundaries() const;

    Drawable Points() const;

    Drawable Triangulate() const;

    Drawable Triangulate(glm::vec2 perspective) const;

    Drawable Wireframe() const;

    Drawable Wireframe(glm::vec2 perspective) const;

  private:
    static constexpr auto kMaxDepth = 12;

    bool FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const;

    Mesh::Face *FindFaceThatContainsPoint(glm::vec2 point) const;

    std::vector<Face *> FindNeighbors(Face *face) const;

    void FindVisibleFaces(glm::vec2 perspective, int max_depth,
                          std::vector<Face *> &visible_faces,
                          std::unordered_map<Face *, float> &depths) const;

    void FindVisibleHalfEdges(glm::vec2 perspective, int max_depth,
                              std::vector<HalfEdge *> &visible_half_edges,
                              std::unordered_map<Face *, float> &depths) const;

  private:
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<HalfEdge>> half_edges;
    std::vector<std::unique_ptr<Vertex>> vertices;
    std::vector<std::unique_ptr<RoomInfo>> room_infos;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESH_H_
