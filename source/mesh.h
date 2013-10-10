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

    Drawable Points() const;

    Drawable Triangulate() const;

    Drawable Wireframe() const;

  private:
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<HalfEdge>> half_edges;
    std::vector<std::unique_ptr<Vertex>> vertices;
    std::vector<std::unique_ptr<RoomInfo>> room_infos;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESH_H_
