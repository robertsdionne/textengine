#ifndef TEXTENGINE_MESHSERIALIZER_H_
#define TEXTENGINE_MESHSERIALIZER_H_

#include <picojson.h>
#include <string>

namespace textengine {

  class Mesh;

  class MeshSerializer {
  public:
    MeshSerializer() = default;

    virtual ~MeshSerializer() = default;

    void WriteMesh(const std::string &filename, const Mesh &mesh) const;

  private:
    double FindVertexIndex(const Mesh &mesh, const Mesh::Vertex *vertex) const;

    double FindHalfEdgeIndex(const Mesh &mesh, const Mesh::HalfEdge *half_edge) const;

    double FindFaceIndex(const Mesh &mesh, const Mesh::Face *face) const;

    picojson::value WriteVertex(const Mesh &mesh, const Mesh::Vertex *vertex) const;

    picojson::value WriteHalfEdge(const Mesh &mesh, const Mesh::HalfEdge *half_edge) const;

    picojson::value WriteFace(const Mesh &mesh, const Mesh::Face *face) const;

    picojson::value WriteVec2(glm::vec2 vector) const;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MESHSERIALIZER_H_
