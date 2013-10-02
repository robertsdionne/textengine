#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "checks.h"
#include "mesh.h"
#include "meshloader.h"

namespace textengine {

  Mesh MeshLoader::ReadMesh(const std::string &filename) const {
    std::ifstream in(filename);
    CHECK_STATE(!in.fail());
    picojson::value value;
    in >> value;
    in.close();
    const std::string error = picojson::get_last_error();
    if (!error.empty()) {
      FAIL(error);
    }
    CHECK_STATE(value.is<picojson::object>());
    picojson::object object = value.get<picojson::object>();
    CHECK_STATE(object["faces"].is<picojson::array>());
    CHECK_STATE(object["half_edges"].is<picojson::array>());
    CHECK_STATE(object["vertices"].is<picojson::array>());
    picojson::array faces_in = object["faces"].get<picojson::array>();
    picojson::array half_edges_in = object["half_edges"].get<picojson::array>();
    picojson::array vertices_in = object["vertices"].get<picojson::array>();
    std::vector<std::unique_ptr<Mesh::Face>> faces_out;
    std::vector<std::unique_ptr<Mesh::HalfEdge>> half_edges_out;
    std::vector<std::unique_ptr<Mesh::Vertex>> vertices_out;
    for (auto i = 0; i < faces_in.size(); ++i) {
      faces_out.emplace_back(new Mesh::Face);
    }
    for (auto i = 0; i < half_edges_in.size(); ++i) {
      half_edges_out.emplace_back(new Mesh::HalfEdge);
    }
    for (auto i = 0; i < vertices_in.size(); ++i) {
      vertices_out.emplace_back(new Mesh::Vertex);
    }
    for (auto i = 0; i < faces_in.size(); ++i) {
      ReadFace(faces_in[i], half_edges_out, faces_out[i].get());
    }
    for (auto i = 0; i < half_edges_in.size(); ++i) {
      ReadHalfEdge(half_edges_in[i], faces_out, half_edges_out,
                   vertices_out, half_edges_out[i].get());
    }
    for (auto i = 0; i < vertices_in.size(); ++i) {
      ReadVertex(vertices_in[i], half_edges_out, vertices_out[i].get());
    }
    return Mesh(std::move(faces_out), std::move(half_edges_out), std::move(vertices_out));
  }

  void MeshLoader::ReadVertex(const picojson::value &vertex_in,
                              const std::vector<std::unique_ptr<Mesh::HalfEdge>> &half_edges,
                              Mesh::Vertex *vertex_out) const {
    CHECK_STATE(vertex_in.is<picojson::object>());
    picojson::object object = vertex_in.get<picojson::object>();
    CHECK_STATE(object["vertex_edge"].is<double>());
    CHECK_STATE(object["position"].is<picojson::object>());
    const long vertex_edge_index = static_cast<long>(object["vertex_edge"].get<double>());
    vertex_out->vertex_edge = vertex_edge_index < 0 ? nullptr : half_edges[vertex_edge_index].get();
    vertex_out->position = ReadVec2(object["position"]);
  }

  void MeshLoader::ReadHalfEdge(const picojson::value &half_edge_in,
                                const std::vector<std::unique_ptr<Mesh::Face>> &faces,
                                const std::vector<std::unique_ptr<Mesh::HalfEdge>> &half_edges,
                                const std::vector<std::unique_ptr<Mesh::Vertex>> &vertices,
                                Mesh::HalfEdge *half_edge_out) const {
    CHECK_STATE(half_edge_in.is<picojson::object>());
    picojson::object object = half_edge_in.get<picojson::object>();
    CHECK_STATE(object["face"].is<double>());
    CHECK_STATE(object["next"].is<double>());
    CHECK_STATE(object["opposite"].is<double>());
    CHECK_STATE(object["previous"].is<double>());
    CHECK_STATE(object["start"].is<double>());
    const long face_index = static_cast<long>(object["face"].get<double>());
    const long next_index = static_cast<long>(object["next"].get<double>());
    const long opposite_index = static_cast<long>(object["opposite"].get<double>());
    const long previous_index = static_cast<long>(object["previous"].get<double>());
    const long start_index = static_cast<long>(object["start"].get<double>());
    half_edge_out->face = face_index < 0 ? nullptr : faces[face_index].get();
    half_edge_out->next = next_index < 0 ? nullptr : half_edges[next_index].get();
    half_edge_out->opposite = opposite_index < 0 ? nullptr : half_edges[opposite_index].get();
    half_edge_out->previous = previous_index < 0 ? nullptr : half_edges[previous_index].get();
    half_edge_out->start = start_index < 0 ? nullptr : vertices[start_index].get();
  }

  void MeshLoader::ReadFace(const picojson::value &face_in,
                            const std::vector<std::unique_ptr<Mesh::HalfEdge>> &half_edges,
                            Mesh::Face *face_out) const {
    CHECK_STATE(face_in.is<picojson::object>());
    picojson::object object = face_in.get<picojson::object>();
    CHECK_STATE(object["face_edge"].is<double>());
    const long face_edge_index = static_cast<long>(object["face_edge"].get<double>());
    face_out->face_edge = face_edge_index < 0 ? nullptr : half_edges[face_edge_index].get();
  }

  glm::vec2 MeshLoader::ReadVec2(const picojson::value &vector) const {
    CHECK_STATE(vector.is<picojson::object>());
    picojson::object object = vector.get<picojson::object>();
    CHECK_STATE(object["x"].is<double>());
    CHECK_STATE(object["y"].is<double>());
    return glm::vec2(object["x"].get<double>(), object["y"].get<double>());
  }

}  // namespace textengine
