#include <fstream>
#include <string>

#include "checks.h"
#include "mesh.h"
#include "meshserializer.h"

namespace textengine {

  void MeshSerializer::WriteMesh(const std::string &filename, Mesh &mesh) const {
    std::ofstream out(filename);
    CHECK_STATE(!out.fail());
    VertexIndexMap vertex_indices = {{nullptr, -1}};
    for (auto i = 0; i < mesh.get_vertices().size(); ++i) {
      vertex_indices.insert({mesh.get_vertices()[i].get(), i});
    }
    HalfEdgeIndexMap half_edge_indices = {{nullptr, -1}};
    for (auto i = 0; i < mesh.get_half_edges().size(); ++i) {
      half_edge_indices.insert({mesh.get_half_edges()[i].get(), i});
    }
    FaceIndexMap face_indices = {{nullptr, -1}};
    for (auto i = 0; i < mesh.get_faces().size(); ++i) {
      face_indices.insert({mesh.get_faces()[i].get(), i});
    }
    RoomInfoIndexMap room_info_indices = {{nullptr, -1}};
    for (auto i = 0; i < mesh.get_room_infos().size(); ++i) {
      room_info_indices.insert({mesh.get_room_infos()[i].get(), i});
    }
    picojson::array vertices;
    for (auto &vertex : mesh.get_vertices()) {
      vertices.push_back(WriteVertex(half_edge_indices, vertex.get()));
    }
    picojson::array half_edges;
    for (auto &half_edge : mesh.get_half_edges()) {
      half_edges.push_back(WriteHalfEdge(face_indices, half_edge_indices,
                                         vertex_indices, half_edge.get()));
    }
    picojson::array faces;
    for (auto &face : mesh.get_faces()) {
      faces.push_back(WriteFace(half_edge_indices, room_info_indices, face.get()));
    }
    picojson::array room_infos;
    for (auto &room_info : mesh.get_room_infos()) {
      room_infos.push_back(WriteRoomInfo(room_info.get()));
    }
    picojson::object object;
    object["vertices"] = picojson::value(vertices);
    object["half_edges"] = picojson::value(half_edges);
    object["faces"] = picojson::value(faces);
    object["room_infos"] = picojson::value(room_infos);
    out << picojson::value(object) << std::endl;
    out.close();
  }

  picojson::value MeshSerializer::WriteVertex(const HalfEdgeIndexMap &half_edge_indices,
                                              const Mesh::Vertex *vertex) const {
    picojson::object object;
    object["position"] = WriteVec2(vertex->position);
    object["vertex_edge"] = picojson::value(half_edge_indices.at(vertex->vertex_edge));
    return picojson::value(object);
  }

  picojson::value MeshSerializer::WriteHalfEdge(const FaceIndexMap &face_indices,
                                                const HalfEdgeIndexMap &half_edge_indices,
                                                const VertexIndexMap &vertex_indices,
                                                const Mesh::HalfEdge *half_edge) const {
    picojson::object object;
    object["face"] = picojson::value(face_indices.at(half_edge->face));
    object["next"] = picojson::value(half_edge_indices.at(half_edge->next));
    object["opposite"] = picojson::value(half_edge_indices.at(half_edge->opposite));
    object["previous"] = picojson::value(half_edge_indices.at(half_edge->previous));
    object["start"] = picojson::value(vertex_indices.at(half_edge->start));
    object["generative"] = picojson::value(half_edge->generative);
    object["obstacle"] = picojson::value(half_edge->obstacle);
    return picojson::value(object);
  }

  picojson::value MeshSerializer::WriteFace(const HalfEdgeIndexMap &half_edge_indices,
                                            const RoomInfoIndexMap &room_info_indices,
                                            const Mesh::Face *face) const {
    picojson::object object;
    object["face_edge"] = picojson::value(half_edge_indices.at(face->face_edge));
    object["room_info"] = picojson::value(room_info_indices.at(face->room_info));
    return picojson::value(object);
  }

  picojson::value MeshSerializer::WriteRoomInfo(const Mesh::RoomInfo *room_info) const {
    picojson::object object;
    object["name"] = picojson::value(room_info->name);
    object["color"] = WriteColor(room_info->color);
    return picojson::value(object);
  }

  picojson::value MeshSerializer::WriteVec2(glm::vec2 vector) const {
    picojson::object object;
    object["x"] = picojson::value(vector.x);
    object["y"] = picojson::value(vector.y);
    return picojson::value(object);
  }

  picojson::value MeshSerializer::WriteColor(glm::vec4 color) const {
    picojson::object object;
    object["r"] = picojson::value(color.r);
    object["g"] = picojson::value(color.g);
    object["b"] = picojson::value(color.b);
    object["a"] = picojson::value(color.a);
    return picojson::value(object);
  }

}  // namespace textengine
