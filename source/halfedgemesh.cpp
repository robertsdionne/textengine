#include <limits>
#include <tuple>

#include "checks.h"
#include "halfedgemesh.h"

namespace textengine {

  Face::Face() : face_edge() {}

  bool Face::IsDummy() const {
    return face_edge->IsDummy();
  }

  HalfEdge::HalfEdge() : face(), next(), opposite(), previous(), start() {}

  bool HalfEdge::IsDummy() const {
    return !start || !next->IsDummy();
  }

  Vertex::Vertex() : position(), vertex_edge() {}

  Vertex::Vertex(glm::vec2 position, HalfEdge *vertex_edge)
  : position(position), vertex_edge(vertex_edge) {}

  HalfEdgeMesh::HalfEdgeMesh() : faces(), half_edges(), vertices(),
      infinity(glm::vec2(std::numeric_limits<float>::infinity()), nullptr) {
    const auto point0 = glm::vec2(-0.5f, 0.0f),
        point1 = glm::vec2(0.5f, 0.0f),
        point2 = glm::vec2(0.0f, glm::sqrt(3.0f));
    HalfEdge *half_edge01, *half_edge12, *half_edge20;
    Vertex *vertex0, *vertex1, *vertex2;
    std::tie(std::ignore, half_edge01, half_edge12, half_edge20,
             vertex0, vertex1, vertex2) = CreateFace(point0, point1, point2);
    HalfEdge *half_edge0infinity, *half_edgeinfinity1;
    std::tie(std::ignore, std::ignore, half_edge0infinity, half_edgeinfinity1, std::ignore,
             std::ignore, std::ignore) = CreateFace(vertex1, vertex0, &infinity, half_edge01);
    HalfEdge *half_edge1infinity, *half_edgeinfinity2;
    std::tie(std::ignore, std::ignore, half_edge1infinity, half_edgeinfinity2, std::ignore,
             std::ignore, std::ignore) = CreateFace(vertex2, vertex1,
                                                    &infinity, half_edge12, half_edgeinfinity1);
    CreateFace(vertex0, vertex2, nullptr, half_edge20, half_edgeinfinity2, half_edge0infinity);
  }

  HalfEdgeMesh::CreationInfo HalfEdgeMesh::CreateFace(glm::vec2 point0, glm::vec2 point1,
                                                      glm::vec2 point2,
                                                      HalfEdge *half_edge10,
                                                      HalfEdge *half_edge21,
                                                      HalfEdge *half_edge02) {
    auto vertex0 = new Vertex, vertex1 = new Vertex, vertex2 = new Vertex;
    vertex0->position = point0;
    vertex1->position = point1;
    vertex2->position = point2;
    return CreateFace(vertex0, vertex1, vertex2, half_edge10, half_edge21, half_edge02);
  }

  HalfEdgeMesh::CreationInfo HalfEdgeMesh::CreateFace(Vertex *vertex0, Vertex *vertex1,
                                                      Vertex *vertex2,
                                                      HalfEdge *half_edge10,
                                                      HalfEdge *half_edge21,
                                                      HalfEdge *half_edge02) {
    auto half_edge01 = new HalfEdge, half_edge12 = new HalfEdge, half_edge20 = new HalfEdge;
    auto face = new Face;

    vertex0->vertex_edge = half_edge01;
    vertex1->vertex_edge = half_edge12;
    vertex2->vertex_edge = half_edge20;

    half_edge01->face = face;
    half_edge01->next = half_edge12;
    half_edge01->opposite = half_edge10;
    half_edge01->previous = half_edge20;
    half_edge01->start = vertex0;

    if (half_edge10) {
      CHECK_STATE(!half_edge10->opposite);
      half_edge10->opposite = half_edge01;
    }

    half_edge12->face = face;
    half_edge12->next = half_edge20;
    half_edge12->opposite = half_edge21;
    half_edge12->previous = half_edge01;
    half_edge12->start = vertex1;

    if (half_edge21) {
      CHECK_STATE(!half_edge21->opposite);
      half_edge21->opposite = half_edge12;
    }

    half_edge20->face = face;
    half_edge20->next = half_edge01;
    half_edge20->opposite = half_edge02;
    half_edge20->previous = half_edge12;
    half_edge20->start = vertex2;

    if (half_edge02) {
      CHECK_STATE(!half_edge02->opposite);
      half_edge02->opposite = half_edge20;
    }

    face->face_edge = half_edge01;

    vertices.emplace(vertex0);
    vertices.emplace(vertex1);
    vertices.emplace(vertex2);

    half_edges.emplace(half_edge01);
    half_edges.emplace(half_edge12);
    half_edges.emplace(half_edge20);
    
    faces.emplace(face);
    return std::make_tuple(face, half_edge01, half_edge12, half_edge20, vertex0, vertex1, vertex2);
  }

}  // namespace textengine
