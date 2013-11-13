#ifndef TEXTENGINE_MESH_H_
#define TEXTENGINE_MESH_H_

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <random>
#include <vector>

#include "drawable.h"

namespace textengine {

  class Mesh {
  public:
    struct HalfEdge;
    struct RoomInfo;
    struct Vertex;

    struct Face {
      HalfEdge *face_edge;
      RoomInfo *room_info;

      glm::vec2 centroid() const;

      void ForEachFace(std::function<void(Face *)> body) const;

      void ForEachHalfEdge(std::function<void(HalfEdge *)> body) const;

      void ForEachVertex(std::function<void(Vertex *)> body) const;
    };

    struct Vertex {
      Vertex() = default;

      HalfEdge *vertex_edge;
      glm::vec2 position;
    };

    struct HalfEdge {
      HalfEdge() = default;

      Face *face;
      HalfEdge *next, *opposite, *previous;
      Vertex *start;
      bool seen, generative, transparent;
    };

    struct RoomInfo {
      RoomInfo() = default;

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

    std::vector<HalfEdge *> Exterior() const;

    void ExtrudeGenerativeEdges(glm::vec2 perspective);

    Drawable Points() const;

    Drawable Shadows(glm::vec2 perspective) const;

    Drawable Triangulate() const;

    Drawable Triangulate(glm::vec2 perspective) const;

    Drawable Wireframe() const;

    Drawable Wireframe(glm::vec2 perspective) const;

    Drawable WireframeExterior() const;

  private:
    static constexpr auto kMaxDepth = 12;

    bool FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const;

    Mesh::Face *FindFaceThatContainsPoint(glm::vec2 point) const;

    void FindVisibleFaces(glm::vec2 perspective, int max_depth,
                          std::vector<Face *> &visible_faces,
                          std::unordered_map<Face *, float> &depths) const;

    void FindVisibleHalfEdges(glm::vec2 perspective, int max_depth,
                              std::vector<HalfEdge *> &visible_half_edges,
                              std::unordered_map<Face *, float> &depths) const;

    void ExtrudeGenerativeEdge(HalfEdge *edge);

  private:
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<HalfEdge>> half_edges;
    std::vector<std::unique_ptr<Vertex>> vertices;
    std::vector<std::unique_ptr<RoomInfo>> room_infos;

    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distribution;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESH_H_
