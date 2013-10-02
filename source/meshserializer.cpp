#include <fstream>

#include "checks.h"
#include "mesh.h"
#include "meshserializer.h"

namespace textengine {

  void MeshSerializer::WriteMesh(const std::string &filename, const Mesh &mesh) const {
    std::ofstream out(filename);
    CHECK_STATE(!out.fail());
    picojson::array vertices;
    for (auto &vertex : mesh.get_vertices()) {
      vertices.push_back(WriteVertex(mesh, vertex.get()));
    }
    picojson::array half_edges;
    for (auto &half_edge : mesh.get_half_edges()) {
      half_edges.push_back(WriteHalfEdge(mesh, half_edge.get()));
    }
    picojson::array faces;
    for (auto &face : mesh.get_faces()) {
      faces.push_back(WriteFace(mesh, face.get()));
    }
    picojson::object object;
    object["vertices"] = picojson::value(vertices);
    object["half_edges"] = picojson::value(half_edges);
    object["faces"] = picojson::value(faces);
    out << picojson::value(object);
    out.close();
  }

  double MeshSerializer::FindVertexIndex(const textengine::Mesh &mesh,
                                    const Mesh::Vertex *vertex) const {
    for (auto i = 0; i < mesh.get_vertices().size(); ++i) {
      if (vertex == mesh.get_vertices()[i].get()) {
        return i;
      }
    }
    return -1.0;
  }

  double MeshSerializer::FindHalfEdgeIndex(const textengine::Mesh &mesh,
                                    const Mesh::HalfEdge *half_edge) const {
    for (auto i = 0; i < mesh.get_half_edges().size(); ++i) {
      if (half_edge == mesh.get_half_edges()[i].get()) {
        return i;
      }
    }
    return -1.0;
  }

  double MeshSerializer::FindFaceIndex(const textengine::Mesh &mesh, const Mesh::Face *face) const {
    for (auto i = 0; i < mesh.get_faces().size(); ++i) {
      if (face == mesh.get_faces()[i].get()) {
        return i;
      }
    }
    return -1.0;
  }

  picojson::value MeshSerializer::WriteVertex(const Mesh &mesh, const Mesh::Vertex *vertex) const {
    picojson::object object;
    object["position"] = WriteVec2(vertex->position);
    object["vertex_edge"] = picojson::value(FindHalfEdgeIndex(mesh, vertex->vertex_edge));
    return picojson::value(object);
  }

  picojson::value MeshSerializer::WriteHalfEdge(const Mesh &mesh,
                                                const Mesh::HalfEdge *half_edge) const {
    picojson::object object;
    object["face"] = picojson::value(FindFaceIndex(mesh, half_edge->face));
    object["next"] = picojson::value(FindHalfEdgeIndex(mesh, half_edge->next));
    object["opposite"] = picojson::value(FindHalfEdgeIndex(mesh, half_edge->opposite));
    object["previous"] = picojson::value(FindHalfEdgeIndex(mesh, half_edge->previous));
    object["start"] = picojson::value(FindVertexIndex(mesh, half_edge->start));
    return picojson::value(object);
  }

  picojson::value MeshSerializer::WriteFace(const Mesh &mesh, const Mesh::Face *face) const {
    picojson::object object;
    object["face_edge"] = picojson::value(FindHalfEdgeIndex(mesh, face->face_edge));
    return picojson::value(object);
  }

  picojson::value MeshSerializer::WriteVec2(glm::vec2 vector) const {
    picojson::object object;
    object["x"] = picojson::value(vector.x);
    object["y"] = picojson::value(vector.y);
    return picojson::value(object);
  }

}  // namespace textengine
