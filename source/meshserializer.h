#ifndef TEXTENGINE_MESHSERIALIZER_H_
#define TEXTENGINE_MESHSERIALIZER_H_

#include <picojson.h>
#include <string>
#include <unordered_map>

#include "mesh.h"

namespace textengine {

  class MeshSerializer {
  public:
    MeshSerializer() = default;

    virtual ~MeshSerializer() = default;

    void WriteMesh(const std::string &filename, Mesh &mesh) const;

  private:
    typedef std::unordered_map<Mesh::Face *, double> FaceIndexMap;
    typedef std::unordered_map<Mesh::HalfEdge *, double> HalfEdgeIndexMap;
    typedef std::unordered_map<Mesh::Vertex *, double> VertexIndexMap;

    picojson::value WriteVertex(const HalfEdgeIndexMap &half_edge_indices,
                                const Mesh::Vertex *vertex) const;

    picojson::value WriteHalfEdge(const FaceIndexMap &face_indices,
                                  const HalfEdgeIndexMap &half_edge_indices,
                                  const VertexIndexMap &vertex_indices,
                                  const Mesh::HalfEdge *half_edge) const;

    picojson::value WriteFace(const HalfEdgeIndexMap &half_edge_indices,
                              const Mesh::Face *face) const;

    picojson::value WriteVec2(glm::vec2 vector) const;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESHSERIALIZER_H_
