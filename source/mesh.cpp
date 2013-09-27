#include <algorithm>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_set>

#include "checks.h"
#include "drawable.h"
#include "mesh.h"

namespace textengine {

  const std::vector<std::unique_ptr<Mesh::HalfEdge>> &Mesh::get_half_edges() const {
    return half_edges;
  }

  const std::vector<std::unique_ptr<Mesh::Face>> &Mesh::get_faces() const {
    return faces;
  }

  const std::vector<std::unique_ptr<Mesh::Vertex>> &Mesh::get_vertices() const {
    return vertices;
  }
  
  void Mesh::AddDefaultFace(glm::vec2 position) {
    const auto point0 = glm::vec2(0, 0), point1 = glm::vec2(0.1, 0), point2 = glm::vec2(0, 0.1);
    auto vertex0 = new Vertex, vertex1 = new Vertex, vertex2 = new Vertex;
    auto half_edge01 = new HalfEdge, half_edge12 = new HalfEdge, half_edge20 = new HalfEdge;
    auto face = new Face;

    vertex0->vertex_edge = half_edge01;
    vertex0->position = position + point0;

    vertex1->vertex_edge = half_edge12;
    vertex1->position = position + point1;
    
    vertex2->vertex_edge = half_edge20;
    vertex2->position = position + point2;

    half_edge01->face = face;
    half_edge01->next = half_edge12;
    half_edge01->opposite = nullptr;
    half_edge01->previous = half_edge20;
    half_edge01->start = vertex0;

    half_edge12->face = face;
    half_edge12->next = half_edge20;
    half_edge12->opposite = nullptr;
    half_edge12->previous = half_edge01;
    half_edge12->start = vertex1;

    half_edge20->face = face;
    half_edge20->next = half_edge01;
    half_edge20->opposite = nullptr;
    half_edge20->previous = half_edge12;
    half_edge20->start = vertex2;

    face->face_edge = half_edge01;

    vertices.emplace_back(vertex0);
    vertices.emplace_back(vertex1);
    vertices.emplace_back(vertex2);

    half_edges.emplace_back(half_edge01);
    half_edges.emplace_back(half_edge12);
    half_edges.emplace_back(half_edge20);

    faces.emplace_back(face);
  }

  void Mesh::AddFace(Vertex *vertex0, Vertex *vertex1, Vertex *vertex2) {
    std::cout << std::endl;
    std::unordered_set<const Vertex *> face_vertices;
    face_vertices.insert(vertex0);
    face_vertices.insert(vertex1);
    face_vertices.insert(vertex2);
    for (auto &face : faces) {
      if (face_vertices.end() != face_vertices.find(face->face_edge->start) &&
          face_vertices.end() != face_vertices.find(face->face_edge->next->start) &&
          face_vertices.end() != face_vertices.find(face->face_edge->next->next->start)) {
        // Face already exists
        std::cout << "face already exists" << std::endl;
        return;
      }
    }

    glm::vec3 d01 = glm::vec3(vertex1->position - vertex0->position, 0.0f);
    glm::vec3 d02 = glm::vec3(vertex2->position - vertex0->position, 0.0f);
    if (glm::cross(d01, d02).z < 0) {
      std::cout << "swapping vertex1 and vertex2 to preserve right-hand rule" << std::endl;
      std::swap(vertex1, vertex2);
    }

    HalfEdge *half_edge10 = nullptr, *half_edge21 = nullptr, *half_edge02 = nullptr;
    for (auto &edge : half_edges) {
      if (vertex0 == edge->start && vertex1 == edge->next->start) {
        // Edge already exists
        std::cout << "edge01 already exists" << std::endl;
        return;
      }
      if (vertex1 == edge->start && vertex0 == edge->next->start) {
        std::cout << "found opposite edge10" << std::endl;
        half_edge10 = edge.get();
      }
      if (vertex1 == edge->start && vertex2 == edge->next->start) {
        // Edge already exists
        std::cout << "edge12 already exists" << std::endl;
        return;
      }
      if (vertex2 == edge->start && vertex1 == edge->next->start) {
        std::cout << "found opposite edge21" << std::endl;
        half_edge21 = edge.get();
      }
      if (vertex2 == edge->start && vertex0 == edge->next->start) {
        // Edge already exists
        std::cout << "edge20 already exists" << std::endl;
        return;
      }
      if (vertex0 == edge->start && vertex2 == edge->next->start) {
        std::cout << "found opposite edge02" << std::endl;
        half_edge02 = edge.get();
      }
    }
    
    auto half_edge01 = new HalfEdge, half_edge12 = new HalfEdge, half_edge20 = new HalfEdge;
    auto face = new Face;

    half_edge01->face = face;
    half_edge01->next = half_edge12;
    half_edge01->opposite = half_edge10;
    half_edge01->previous = half_edge20;
    half_edge01->start = vertex0;

    if (half_edge10) {
      half_edge10->opposite = half_edge01;
    }

    half_edge12->face = face;
    half_edge12->next = half_edge20;
    half_edge12->opposite = half_edge21;
    half_edge12->previous = half_edge01;
    half_edge12->start = vertex1;

    if (half_edge21) {
      half_edge21->opposite = half_edge12;
    }

    half_edge20->face = face;
    half_edge20->next = half_edge01;
    half_edge20->opposite = half_edge02;
    half_edge20->previous = half_edge12;
    half_edge20->start = vertex2;

    if (half_edge02) {
      half_edge02->opposite = half_edge20;
    }

    face->face_edge = half_edge01;

    half_edges.emplace_back(half_edge01);
    half_edges.emplace_back(half_edge12);
    half_edges.emplace_back(half_edge20);
    
    faces.emplace_back(face);
  }

  Mesh::Vertex *Mesh::ExtrudeEdge(Mesh::HalfEdge *edge) {
    if (!edge->opposite) {
      const auto point0 = edge->next->start->position, point1 = edge->start->position;
      const auto point2 = (point0 + point1) / 2.0f;
      auto vertex0 = edge->next->start, vertex1 = edge->start, vertex2 = new Vertex;
      auto half_edge01 = new HalfEdge, half_edge12 = new HalfEdge, half_edge20 = new HalfEdge;
      auto face = new Face;

      vertex0->vertex_edge = half_edge01;

      vertex1->vertex_edge = half_edge12;

      vertex2->vertex_edge = half_edge20;
      vertex2->position = point2;

      edge->opposite = half_edge01;

      half_edge01->face = face;
      half_edge01->next = half_edge12;
      half_edge01->opposite = edge;
      half_edge01->previous = half_edge20;
      half_edge01->start = vertex0;

      half_edge12->face = face;
      half_edge12->next = half_edge20;
      half_edge12->opposite = nullptr;
      half_edge12->previous = half_edge01;
      half_edge12->start = vertex1;

      half_edge20->face = face;
      half_edge20->next = half_edge01;
      half_edge20->opposite = nullptr;
      half_edge20->previous = half_edge12;
      half_edge20->start = vertex2;

      face->face_edge = half_edge01;

      vertices.emplace_back(vertex2);

      half_edges.emplace_back(half_edge01);
      half_edges.emplace_back(half_edge12);
      half_edges.emplace_back(half_edge20);
      
      faces.emplace_back(face);
      
      return vertex2;
    } else {
      return nullptr;
    }
  }

  Drawable Mesh::Points() const {
    Drawable drawable;
    constexpr size_t kCoordinatesPerVertex = 2;
    drawable.data_size = kCoordinatesPerVertex * vertices.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    for (auto i = 0; i < vertices.size(); ++i) {
      drawable.data[kCoordinatesPerVertex * i + 0] = vertices[i]->position.x;
      drawable.data[kCoordinatesPerVertex * i + 1] = vertices[i]->position.y;
    }
    drawable.element_count = static_cast<GLsizei>(vertices.size());
    drawable.element_type = GL_POINTS;
    return drawable;
  }

  Drawable Mesh::Triangulate() const {
    Drawable drawable;
    constexpr size_t kVerticesPerFace = 3;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kFaceSize = kVerticesPerFace * kCoordinatesPerVertex;
    drawable.data_size = kFaceSize * faces.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    for (auto i = 0; i < faces.size(); ++i) {
      const auto h01 = faces[i]->face_edge;
      const auto h12 = h01->next;
      const auto h20 = h12->next;
      CHECK_STATE(h01 == h20->next);
      const auto v0 = h01->start, v1 = h12->start, v2 = h20->start;
      drawable.data[kFaceSize * i + 0] = v0->position.x;
      drawable.data[kFaceSize * i + 1] = v0->position.y;
      drawable.data[kFaceSize * i + 2] = v1->position.x;
      drawable.data[kFaceSize * i + 3] = v1->position.y;
      drawable.data[kFaceSize * i + 4] = v2->position.x;
      drawable.data[kFaceSize * i + 5] = v2->position.y;
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerFace * faces.size());
    drawable.element_type = GL_TRIANGLES;
    return drawable;
  }

  Drawable Mesh::Wireframe() const {
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kEdgeSize = kVerticesPerEdge * kCoordinatesPerVertex;
    drawable.data_size = kEdgeSize * half_edges.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    for (auto i = 0; i < half_edges.size(); ++i) {
      drawable.data[kEdgeSize * i + 0] = half_edges[i]->start->position.x;
      drawable.data[kEdgeSize * i + 1] = half_edges[i]->start->position.y;
      drawable.data[kEdgeSize * i + 2] = half_edges[i]->next->start->position.x;
      drawable.data[kEdgeSize * i + 3] = half_edges[i]->next->start->position.y;
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerEdge * half_edges.size());
    drawable.element_type = GL_LINES;
    return drawable;
  }

}  // namespace textengine
