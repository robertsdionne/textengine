#ifndef TEXTENGINE_MESHLOADER_H_
#define TEXTENGINE_MESHLOADER_H_

#include <picojson.h>
#include <string>
#include <unordered_map>

#include "mesh.h"

namespace textengine {

  class MeshLoader {
  public:
    MeshLoader() = default;

    virtual ~MeshLoader() = default;

    Mesh ReadMesh(const std::string &filename) const;

  private:
    void ReadVertex(const picojson::value &vertex_in,
                    const std::vector<std::unique_ptr<Mesh::HalfEdge>> &half_edges,
                    Mesh::Vertex *vertex_out) const;

    void ReadHalfEdge(const picojson::value &half_edge_in,
                      const std::vector<std::unique_ptr<Mesh::Face>> &faces,
                      const std::vector<std::unique_ptr<Mesh::HalfEdge>> &half_edges,
                      const std::vector<std::unique_ptr<Mesh::Vertex>> &vertices,
                      Mesh::HalfEdge *half_edge_out) const;

    void ReadFace(const picojson::value &face_in,
                  const std::vector<std::unique_ptr<Mesh::HalfEdge>> &half_edges,
                  Mesh::Face *face_out) const;

    glm::vec2 ReadVec2(const picojson::value &vector) const;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESHLOADER_H_
