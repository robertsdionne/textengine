#include <algorithm>
#include <deque>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "checks.h"
#include "drawable.h"
#include "mesh.h"

namespace textengine {

  glm::vec2 Mesh::Face::centroid() const {
    auto centroid = glm::vec2();
    auto count = 1.0f;
    ForEachHalfEdge([&] (HalfEdge *half_edge) {
      centroid += (half_edge->start->position - centroid) / count;
      count += 1;
    });
    return centroid;
  }

  void Mesh::Face::ForEachFace(std::function<void(Face *)> body) const {
    auto half_edge = face_edge;
    do {
      if (half_edge->opposite) {
        body(half_edge->opposite->face);
      }
      half_edge = half_edge->next;
    } while (face_edge != half_edge);
  }

  void Mesh::Face::ForEachHalfEdge(std::function<void(HalfEdge *)> body) const {
    auto half_edge = face_edge;
    do {
      body(half_edge);
      half_edge = half_edge->next;
    } while (face_edge != half_edge);
  }

  void Mesh::Face::ForEachVertex(std::function<void(Vertex *)> body) const {
    auto half_edge = face_edge;
    do {
      body(half_edge->start);
      half_edge = half_edge->next;
    } while (face_edge != half_edge);
  }

  Mesh::Mesh(std::vector<std::unique_ptr<Face>> &&faces,
             std::vector<std::unique_ptr<HalfEdge>> &&half_edges,
             std::vector<std::unique_ptr<Vertex>> &&vertices,
             std::vector<std::unique_ptr<RoomInfo>> &&room_infos)
  : faces(std::move(faces)), half_edges(std::move(half_edges)), vertices(std::move(vertices)),
  room_infos(std::move(room_infos)) {}

  std::vector<std::unique_ptr<Mesh::Face>> &Mesh::get_faces() {
    return faces;
  }

  std::vector<std::unique_ptr<Mesh::HalfEdge>> &Mesh::get_half_edges() {
    return half_edges;
  }

  std::vector<std::unique_ptr<Mesh::Vertex>> &Mesh::get_vertices() {
    return vertices;
  }

  std::vector<std::unique_ptr<Mesh::RoomInfo>> &Mesh::get_room_infos() {
    return room_infos;
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
    face->room_info = nullptr;

    vertices.emplace_back(vertex0);
    vertices.emplace_back(vertex1);
    vertices.emplace_back(vertex2);

    half_edges.emplace_back(half_edge01);
    half_edges.emplace_back(half_edge12);
    half_edges.emplace_back(half_edge20);

    faces.emplace_back(face);
  }

  void Mesh::AddFace(Vertex *vertex0, Vertex *vertex1, Vertex *vertex2) {
    std::unordered_set<const Vertex *> face_vertices;
    face_vertices.insert(vertex0);
    face_vertices.insert(vertex1);
    face_vertices.insert(vertex2);
    for (auto &face : faces) {
      if (face_vertices.end() != face_vertices.find(face->face_edge->start) &&
          face_vertices.end() != face_vertices.find(face->face_edge->next->start) &&
          face_vertices.end() != face_vertices.find(face->face_edge->next->next->start)) {
        // Face already exists
        return;
      }
    }

    glm::vec3 d01 = glm::vec3(vertex1->position - vertex0->position, 0.0f);
    glm::vec3 d02 = glm::vec3(vertex2->position - vertex0->position, 0.0f);
    if (glm::cross(d01, d02).z < 0) {
      // swapping vertex1 and vertex2 to preserve right-hand rule
      std::swap(vertex1, vertex2);
    }

    HalfEdge *half_edge10 = nullptr, *half_edge21 = nullptr, *half_edge02 = nullptr;
    for (auto &edge : half_edges) {
      if (vertex0 == edge->start && vertex1 == edge->next->start) {
        // Edge already exists
        return;
      }
      if (vertex1 == edge->start && vertex0 == edge->next->start) {
        // found opposite edge
        half_edge10 = edge.get();
      }
      if (vertex1 == edge->start && vertex2 == edge->next->start) {
        // Edge already exists
        return;
      }
      if (vertex2 == edge->start && vertex1 == edge->next->start) {
        // found opposite edge
        half_edge21 = edge.get();
      }
      if (vertex2 == edge->start && vertex0 == edge->next->start) {
        // Edge already exists
        return;
      }
      if (vertex0 == edge->start && vertex2 == edge->next->start) {
        // found opposite edge
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
    face->room_info = nullptr;

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
      face->room_info = nullptr;

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

  std::vector<std::unique_ptr<std::vector<glm::vec2>>> Mesh::Boundaries() const {
    std::unordered_set<HalfEdge *> unused_border_edges;
    for (auto &half_edge : half_edges) {
      if (!half_edge->opposite) {
        unused_border_edges.insert(half_edge.get());
      }
    }
    std::vector<std::unique_ptr<std::vector<glm::vec2>>> result;
    while (unused_border_edges.size()) {
      auto *first = *unused_border_edges.begin();
      unused_border_edges.erase(first);
      auto *loop = new std::vector<glm::vec2>();
      loop->push_back(first->start->position);
      HalfEdge *next = first->next;
      do {
        if (next->opposite) {
          next = next->opposite->next;
        } else {
          loop->push_back(next->start->position);
          unused_border_edges.erase(next);
          next = next->next;
        }
      } while (first != next);
      result.emplace_back(loop);
    }
    return result;
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
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kFaceSize = kVerticesPerFace * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    drawable.data_size = kFaceSize * faces.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    for (auto i = 0; i < faces.size(); ++i) {
      const glm::vec4 color = faces[i]->room_info ? faces[i]->room_info->color : glm::vec4(glm::vec3(0.64f), 1.0f);
      const auto h01 = faces[i]->face_edge;
      const auto h12 = h01->next;
      const auto h20 = h12->next;
      CHECK_STATE(h01 == h20->next);
      const auto v0 = h01->start, v1 = h12->start, v2 = h20->start;
      drawable.data[kFaceSize * i + 0] = v0->position.x;
      drawable.data[kFaceSize * i + 1] = v0->position.y;
      drawable.data[kFaceSize * i + 2] = color.r;
      drawable.data[kFaceSize * i + 3] = color.g;
      drawable.data[kFaceSize * i + 4] = color.b;
      drawable.data[kFaceSize * i + 5] = color.a;
      drawable.data[kFaceSize * i + 6] = v1->position.x;
      drawable.data[kFaceSize * i + 7] = v1->position.y;
      drawable.data[kFaceSize * i + 8] = color.r;
      drawable.data[kFaceSize * i + 9] = color.g;
      drawable.data[kFaceSize * i + 10] = color.b;
      drawable.data[kFaceSize * i + 11] = color.a;
      drawable.data[kFaceSize * i + 12] = v2->position.x;
      drawable.data[kFaceSize * i + 13] = v2->position.y;
      drawable.data[kFaceSize * i + 14] = color.r;
      drawable.data[kFaceSize * i + 15] = color.g;
      drawable.data[kFaceSize * i + 16] = color.b;
      drawable.data[kFaceSize * i + 17] = color.a;
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerFace * faces.size());
    drawable.element_type = GL_TRIANGLES;
    return drawable;
  }

  Drawable Mesh::Triangulate(glm::vec2 perspective) const {
    auto visible_faces = std::vector<Face *>();
    auto depths = std::unordered_map<Face *, float>();
    FindVisibleFaces(perspective, kMaxDepth, visible_faces, depths);
    Drawable drawable;
    constexpr size_t kVerticesPerFace = 3;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kFaceSize = kVerticesPerFace * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    drawable.data_size = kFaceSize * visible_faces.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    for (auto i = 0; i < visible_faces.size(); ++i) {
      const float brightness = 1.0f - pow(depths.at(visible_faces[i]) / kMaxDepth, 2.0f);
      const glm::vec4 color = brightness * (visible_faces[i]->room_info ? visible_faces[i]->room_info->color : glm::vec4(glm::vec3(0.64f), 1.0f));
      const auto h01 = visible_faces[i]->face_edge;
      const auto h12 = h01->next;
      const auto h20 = h12->next;
      CHECK_STATE(h01 == h20->next);
      const auto v0 = h01->start, v1 = h12->start, v2 = h20->start;
      drawable.data[kFaceSize * i + 0] = v0->position.x;
      drawable.data[kFaceSize * i + 1] = v0->position.y;
      drawable.data[kFaceSize * i + 2] = color.r;
      drawable.data[kFaceSize * i + 3] = color.g;
      drawable.data[kFaceSize * i + 4] = color.b;
      drawable.data[kFaceSize * i + 5] = color.a;
      drawable.data[kFaceSize * i + 6] = v1->position.x;
      drawable.data[kFaceSize * i + 7] = v1->position.y;
      drawable.data[kFaceSize * i + 8] = color.r;
      drawable.data[kFaceSize * i + 9] = color.g;
      drawable.data[kFaceSize * i + 10] = color.b;
      drawable.data[kFaceSize * i + 11] = color.a;
      drawable.data[kFaceSize * i + 12] = v2->position.x;
      drawable.data[kFaceSize * i + 13] = v2->position.y;
      drawable.data[kFaceSize * i + 14] = color.r;
      drawable.data[kFaceSize * i + 15] = color.g;
      drawable.data[kFaceSize * i + 16] = color.b;
      drawable.data[kFaceSize * i + 17] = color.a;
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerFace * visible_faces.size());
    drawable.element_type = GL_TRIANGLES;
    return drawable;
  }

  Drawable Mesh::Wireframe() const {
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kEdgeSize = kVerticesPerEdge * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    drawable.data_size = kEdgeSize * half_edges.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    for (auto i = 0; i < half_edges.size(); ++i) {
      const glm::vec4 color = half_edges[i]->face->room_info ? half_edges[i]->face->room_info->color / 2.0f : glm::vec4(glm::vec3(0.32f), 1.0f);
      drawable.data[kEdgeSize * i + 0] = half_edges[i]->start->position.x;
      drawable.data[kEdgeSize * i + 1] = half_edges[i]->start->position.y;
      drawable.data[kEdgeSize * i + 2] = color.r;
      drawable.data[kEdgeSize * i + 3] = color.g;
      drawable.data[kEdgeSize * i + 4] = color.b;
      drawable.data[kEdgeSize * i + 5] = color.a;
      drawable.data[kEdgeSize * i + 6] = half_edges[i]->next->start->position.x;
      drawable.data[kEdgeSize * i + 7] = half_edges[i]->next->start->position.y;
      drawable.data[kEdgeSize * i + 8] = color.r;
      drawable.data[kEdgeSize * i + 9] = color.g;
      drawable.data[kEdgeSize * i + 10] = color.b;
      drawable.data[kEdgeSize * i + 11] = color.a;
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerEdge * half_edges.size());
    drawable.element_type = GL_LINES;
    return drawable;
  }

  Drawable Mesh::Wireframe(glm::vec2 perspective) const {
    auto visible_half_edges = std::vector<HalfEdge *>();
    auto depths = std::unordered_map<Face *, float>();
    FindVisibleHalfEdges(perspective, kMaxDepth, visible_half_edges, depths);
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kEdgeSize = kVerticesPerEdge * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    drawable.data_size = kEdgeSize * visible_half_edges.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    for (auto i = 0; i < visible_half_edges.size(); ++i) {
      const float brightness = 1.0f - pow(depths.at(visible_half_edges[i]->face) / kMaxDepth, 2.0f);
      const glm::vec4 color = brightness * (visible_half_edges[i]->face->room_info ? visible_half_edges[i]->face->room_info->color / 2.0f : glm::vec4(glm::vec3(0.32f), 1.0f));
      drawable.data[kEdgeSize * i + 0] = visible_half_edges[i]->start->position.x;
      drawable.data[kEdgeSize * i + 1] = visible_half_edges[i]->start->position.y;
      drawable.data[kEdgeSize * i + 2] = color.r * brightness;
      drawable.data[kEdgeSize * i + 3] = color.g * brightness;
      drawable.data[kEdgeSize * i + 4] = color.b * brightness;
      drawable.data[kEdgeSize * i + 5] = color.a;
      drawable.data[kEdgeSize * i + 6] = visible_half_edges[i]->next->start->position.x;
      drawable.data[kEdgeSize * i + 7] = visible_half_edges[i]->next->start->position.y;
      drawable.data[kEdgeSize * i + 8] = color.r * brightness;
      drawable.data[kEdgeSize * i + 9] = color.g * brightness;
      drawable.data[kEdgeSize * i + 10] = color.b * brightness;
      drawable.data[kEdgeSize * i + 11] = color.a;
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerEdge * visible_half_edges.size());
    drawable.element_type = GL_LINES;
    return drawable;
  }

  bool Mesh::FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const {
    const auto h01 = face->face_edge;
    const auto h12 = h01->next;
    const auto h20 = h12->next;
    CHECK_STATE(h01 == h20->next);
    const auto v0 = h01->start, v1 = h12->start, v2 = h20->start;
    const auto p0 = glm::vec3(v0->position, 0.0f), p1 = glm::vec3(v1->position, 0.0f), p2 = glm::vec3(v2->position, 0.0f);
    const auto p = glm::vec3(point, 0.0f);
    const float u = (glm::cross(p, p2-p0).z - glm::cross(p0, p2-p0).z) / glm::cross(p1-p0, p2-p0).z;
    const float v = (glm::cross(p0, p1-p0).z - glm::cross(p, p1-p0).z) / glm::cross(p1-p0, p2-p0).z;
    return 0 <= u && 0 <= v && (u + v) <= 1;
  }

  Mesh::Face *Mesh::FindFaceThatContainsPoint(glm::vec2 point) const {
    for (auto &face : faces) {
      if (FaceContainsPoint(face.get(), point)) {
        return face.get();
      }
    }
    return nullptr;
  }

  void Mesh::FindVisibleFaces(glm::vec2 perspective, int max_depth,
                              std::vector<Face *> &visible_faces,
                              std::unordered_map<Face *, float> &depths) const {
    auto start = FindFaceThatContainsPoint(perspective);
    if (start) {
      auto queue = std::deque<Face *>();
      queue.push_back(start);
      depths.insert({start, 0});
      auto visited = std::unordered_set<Face *>();
      while (queue.size()) {
        auto face = queue.front();
        auto depth = depths.at(face);
        queue.pop_front();
        visible_faces.push_back(face);
        visited.insert(face);
        face->ForEachFace([&] (Face *neighbor) {
          if (visited.end() == visited.find(neighbor) && depth < max_depth) {
            depths.insert({neighbor, depth + 1});
            queue.push_back(neighbor);
          }
        });
      }
    }
  }

  void Mesh::FindVisibleHalfEdges(glm::vec2 perspective, int max_depth,
                                  std::vector<HalfEdge *> &visible_half_edges,
                                  std::unordered_map<Face *, float> &depths) const {
    auto visible_faces = std::vector<Face *>();
    FindVisibleFaces(perspective, max_depth, visible_faces, depths);
    for (auto face : visible_faces) {
      face->ForEachHalfEdge([&] (HalfEdge *half_edge) {
        visible_half_edges.push_back(half_edge);
      });
    }
  }

}  // namespace textengine
