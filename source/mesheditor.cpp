#include <GLFW/glfw3.h>
#include <algorithm>
#include <cctype>
#include <functional>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

#include "checks.h"
#include "drawable.h"
#include "keyboard.h"
#include "mesh.h"
#include "mesheditor.h"
#include "meshloader.h"
#include "meshserializer.h"
#include "mouse.h"

namespace textengine {

  MeshEditor::MeshEditor(int width, int height, Keyboard &keyboard, Mouse &mouse, Mesh &mesh,
                         std::default_random_engine &engine, const std::string &filename)
  : width(width), height(height), keyboard(keyboard), mouse(mouse), mesh(mesh), selected_vertices(),
  additionally_selected_vertices(), selected_vertex_positions(), cursor_start_position(),
  center_of_mass(), model_view_projection(), add_selecting(), rotating(), selecting(), moving(),
  scaling(), engine(engine), filename(filename), uniform_real(), uniform_int(0, 26) {}

  glm::vec2 MeshEditor::get_cursor_position() const {
    const glm::mat4 normalized_to_reversed = glm::scale(glm::mat4(), glm::vec3(1.0f, -1.0f, 1.0f));
    const glm::mat4 reversed_to_offset = glm::translate(glm::mat4(), glm::vec3(glm::vec2(1.0f), 0.0f));
    const glm::mat4 offset_to_screen = glm::scale(glm::mat4(), glm::vec3(glm::vec2(0.5f), 1.0f));
    const glm::mat4 screen_to_window = glm::scale(glm::mat4(), glm::vec3(width, height, 1.0f));
    const glm::vec4 homogeneous = (glm::inverse(screen_to_window * offset_to_screen *
                                                reversed_to_offset * normalized_to_reversed *
                                                model_view_projection) *
                                   glm::vec4(mouse.get_cursor_position(), 0.0f, 1.0f));
    const glm::vec2 transformed = homogeneous.xy() / homogeneous.w;
    return transformed;
  }

  std::unordered_set<Mesh::Vertex *> MeshEditor::potentially_selected_vertices() const {
    std::unordered_set<Mesh::Vertex *> vertex_union;
    std::set_union(selected_vertices.begin(), selected_vertices.end(),
                   additionally_selected_vertices.begin(), additionally_selected_vertices.end(),
                   std::inserter(vertex_union, vertex_union.end()));
    return vertex_union;
  }

  std::unordered_set<Mesh::HalfEdge *> MeshEditor::potentially_selected_half_edges() const {
    std::unordered_set<Mesh::Vertex *> vertices = potentially_selected_vertices();
    std::unordered_set<Mesh::HalfEdge *> half_edges;
    for (const auto &half_edge : mesh.get_half_edges()) {
      if (vertices.end() != vertices.find(half_edge->start) &&
          vertices.end() != vertices.find(half_edge->next->start)) {
        half_edges.insert(half_edge.get());
      }
    }
    return half_edges;
  }

  std::unordered_set<Mesh::Face *> MeshEditor::potentially_selected_faces() const {
    std::unordered_set<Mesh::HalfEdge *> half_edges = potentially_selected_half_edges();
    std::unordered_set<Mesh::Face *> faces;
    for (const auto &face : mesh.get_faces()) {
      const auto h01 = face->face_edge;
      const auto h12 = h01->next;
      const auto h20 = h12->next;
      CHECK_STATE(h01 == h20->next);
      if (half_edges.end() != half_edges.find(h01) &&
          half_edges.end() != half_edges.find(h12) &&
          half_edges.end() != half_edges.find(h20)) {
        faces.insert(face.get());
      }
    }
    return faces;
  }

  std::unordered_set<Mesh::HalfEdge *> MeshEditor::selected_half_edges() const {
    std::unordered_set<Mesh::HalfEdge *> half_edges;
    for (const auto &half_edge : mesh.get_half_edges()) {
      if (selected_vertices.end() != selected_vertices.find(half_edge->start) &&
          selected_vertices.end() != selected_vertices.find(half_edge->next->start)) {
        half_edges.insert(half_edge.get());
      }
    }
    return half_edges;
  }

  std::unordered_set<Mesh::Face *> MeshEditor::selected_faces() const {
    std::unordered_set<Mesh::HalfEdge *> half_edges = selected_half_edges();
    std::unordered_set<Mesh::Face *> faces;
    for (const auto &face : mesh.get_faces()) {
      const auto h01 = face->face_edge;
      const auto h12 = h01->next;
      const auto h20 = h12->next;
      CHECK_STATE(h01 == h20->next);
      if (half_edges.end() != half_edges.find(h01) &&
          half_edges.end() != half_edges.find(h12) &&
          half_edges.end() != half_edges.find(h20)) {
        faces.insert(face.get());
      }
    }
    return faces;
  }

  void MeshEditor::set_model_view_projection(glm::mat4 model_view_projection) {
    MeshEditor::model_view_projection = model_view_projection;
  }

  Mesh::RoomInfo *MeshEditor::CreateRandomizedRoomInfo() {
    auto room_info = new Mesh::RoomInfo;
    char previous = ' ';
    char next = 0;
    while (true) {
      next = uniform_int(engine);
      if (!next) {
        break;
      }
      char current;
      if (1 == next) {
        current = ' ';
      } else {
        current = 'a' + next - 2;
      }
      if (' ' == previous) {
        current = toupper(current);
      }
      room_info->name.push_back(current);
      previous = current;
    }
    room_info->color = glm::vec4(uniform_real(engine), uniform_real(engine), uniform_real(engine), 1.0f);
    return room_info;
  }

  Drawable MeshEditor::HighlightedPoints() const {
    std::unordered_set<Mesh::Vertex *> vertices = potentially_selected_vertices();
    Drawable drawable;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    const auto color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    drawable.data.reserve((kCoordinatesPerVertex + kColorComponentsPerVertex) * vertices.size());
    auto vertex = vertices.begin();
    for (auto i = 0; i < vertices.size(); ++i, ++vertex) {
      drawable.data.insert(drawable.data.cend(), {
        (*vertex)->position.x, (*vertex)->position.y,
        color.r, color.g, color.b, color.a
      });
    }
    drawable.element_count = static_cast<GLsizei>(vertices.size());
    drawable.element_type = GL_POINTS;
    return drawable;
  }

  Drawable MeshEditor::HighlightedTriangles() const {
    std::unordered_set<Mesh::Face *> faces = potentially_selected_faces();
    Drawable drawable;
    constexpr size_t kVerticesPerFace = 3;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kFaceSize = kVerticesPerFace * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    drawable.data.reserve(kFaceSize * faces.size());
    auto face = faces.begin();
    const auto color = glm::vec4(0.32f, 0.0f, 0.0f, 1.0f);
    for (auto i = 0; i < faces.size(); ++i, ++face) {
      const auto h01 = (*face)->face_edge;
      const auto h12 = h01->next;
      const auto h20 = h12->next;
      CHECK_STATE(h01 == h20->next);
      const auto v0 = h01->start, v1 = h12->start, v2 = h20->start;
      drawable.data.insert(drawable.data.cend(), {
        v0->position.x, v0->position.y,
        color.r, color.g, color.b, color.a,
        v1->position.x, v1->position.y,
        color.r, color.g, color.b, color.a,
        v2->position.x, v2->position.y,
        color.r, color.g, color.b, color.a
      });
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerFace * faces.size());
    drawable.element_type = GL_TRIANGLES;
    return drawable;
  }

  Drawable MeshEditor::HighlightedWireframe() const {
    std::unordered_set<Mesh::HalfEdge *> half_edges = potentially_selected_half_edges();
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kEdgeSize = kVerticesPerEdge * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    const size_t interior_edges = std::count_if(half_edges.begin(), half_edges.end(),
                                                [] (Mesh::HalfEdge *half_edge) {
                                                  return half_edge->opposite;
                                                });
    const auto color = glm::vec4(0.64f, 0.0f, 0.0f, 1.0f);
    drawable.data.reserve(kEdgeSize * interior_edges);
    for (auto &half_edge : half_edges) {
      if (half_edge->opposite) {
        drawable.data.insert(drawable.data.cend(), {
          half_edge->start->position.x, half_edge->start->position.y,
          color.r, color.g, color.b, color.a,
          half_edge->next->start->position.x, half_edge->next->start->position.y,
          color.r, color.g, color.b, color.a
        });
      }
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerEdge * interior_edges);
    drawable.element_type = GL_LINES;
    return drawable;
  }

  Drawable MeshEditor::HighlightedWireframeExterior() const {
    std::unordered_set<Mesh::HalfEdge *> half_edges = potentially_selected_half_edges();
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kEdgeSize = kVerticesPerEdge * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    const size_t exterior_edges = std::count_if(half_edges.begin(), half_edges.end(),
                                                [] (Mesh::HalfEdge *half_edge) {
                                                  return !half_edge->opposite;
                                                });
    const auto color = glm::vec4(0.64f, 0.0f, 0.0f, 1.0f);
    drawable.data.reserve(kEdgeSize * exterior_edges);
    for (auto &half_edge : half_edges) {
      if (!half_edge->opposite) {
        drawable.data.insert(drawable.data.cend(), {
          half_edge->start->position.x, half_edge->start->position.y,
          color.r, color.g, color.b, color.a,
          half_edge->next->start->position.x, half_edge->next->start->position.y,
          color.r, color.g, color.b, color.a
        });
      }
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerEdge * exterior_edges);
    drawable.element_type = GL_LINES;
    return drawable;
  }

  Drawable MeshEditor::MoveScaleIndicator() const {
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kEdgeSize = kVerticesPerEdge * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    const glm::vec4 color = glm::vec4(0.64, 0.64, 0.0, 1);
    if (MoveMode::kFalse != moving) {
      drawable.data.reserve(kEdgeSize);
      const glm::vec2 cursor_end_position = get_cursor_position();
      drawable.data.insert(drawable.data.cend(), {
        cursor_start_position.x, cursor_start_position.y,
        color.r, color.g, color.b, color.a
      });
      if (MoveMode::kBoth == moving) {
        drawable.data.insert(drawable.data.cend(), {
          cursor_end_position.x, cursor_end_position.y
        });
      } else if (MoveMode::kX == moving) {
        drawable.data.insert(drawable.data.cend(), {
          cursor_end_position.x, cursor_start_position.y
        });
      } else if (MoveMode::kY == moving) {
        drawable.data.insert(drawable.data.cend(), {
          cursor_start_position.x, cursor_end_position.y
        });
      }
      drawable.data.insert(drawable.data.cend(), {
        color.r, color.g, color.b, color.a,
        cursor_end_position.x, cursor_end_position.y,
        color.r, color.g, color.b, color.a
      });
      drawable.element_count = 2;
    } else if (rotating) {
      drawable.data.reserve(kEdgeSize * 2);
      const glm::vec2 cursor_end_position = get_cursor_position();
      drawable.data.insert(drawable.data.cend(), {
        center_of_mass.x, center_of_mass.y,
        color.r, color.g, color.b, color.a,
        cursor_start_position.x, cursor_start_position.y,
        color.r, color.g, color.b, color.a,
        center_of_mass.x, center_of_mass.y,
        color.r, color.g, color.b, color.a,
        cursor_end_position.x, cursor_end_position.y,
        color.r, color.g, color.b, color.a,
      });
      drawable.element_count = 4;
    } else if (ScaleMode::kFalse != scaling) {
      drawable.data.reserve(kEdgeSize);
      const glm::vec2 cursor_end_position = get_cursor_position();
      drawable.data.insert(drawable.data.cend(), {
        center_of_mass.x, center_of_mass.y,
        color.r, color.g, color.b, color.a
      });
      if (ScaleMode::kAll == scaling || ScaleMode::kBoth == scaling) {
        drawable.data.insert(drawable.data.cend(), {
          cursor_end_position.x, cursor_end_position.y
        });
      } else if (ScaleMode::kX == scaling) {
        drawable.data.insert(drawable.data.cend(), {
          cursor_end_position.x, center_of_mass.y
        });
      } else if (ScaleMode::kY == scaling) {
        drawable.data.insert(drawable.data.cend(), {
          center_of_mass.x, cursor_end_position.y
        });
      }
      drawable.data.insert(drawable.data.cend(), {
        color.r, color.g, color.b, color.a
      });
      drawable.element_count = 2;
    } else {
      drawable.element_count = 0;
    }
    drawable.element_type = GL_LINES;
    return drawable;
  }

  Drawable MeshEditor::PathfindingEdges() const {
    Drawable drawable;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kEdgeSize = kVerticesPerEdge * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    const size_t interior_edges = std::count_if(mesh.get_half_edges().begin(),
                                                mesh.get_half_edges().end(),
                                                [] (const std::unique_ptr<Mesh::HalfEdge> &half_edge) {
                                                  return half_edge->opposite;
                                                });
    drawable.data.reserve(kEdgeSize * interior_edges);
    for (auto &half_edge : mesh.get_half_edges()) {
      if (half_edge->opposite) {
        const auto centroid = half_edge->face->centroid();
        const auto opposite_centroid = half_edge->opposite->face->centroid();
        const auto color = half_edge->face->room_info ? half_edge->face->room_info->color / 2.0f : glm::vec4(0.0f, 0.0f, 0.64f, 1.0f);
        const auto opposite_color = half_edge->opposite->face->room_info ? half_edge->opposite->face->room_info->color / 2.0f : glm::vec4(0.0f, 0.0f, 0.64f, 1.0f);
        drawable.data.insert(drawable.data.cend(), {
          centroid.x, centroid.y,
          color.r, color.g, color.b, color.a,
          opposite_centroid.x, opposite_centroid.y,
          opposite_color.r, opposite_color.g, opposite_color.b, opposite_color.a
        });
      }
    }
    drawable.element_count = static_cast<GLsizei>(kVerticesPerEdge * interior_edges);
    drawable.element_type = GL_LINES;
    return drawable;
  }

  Drawable MeshEditor::PathfindingNodes() const {
    Drawable drawable;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    drawable.data.reserve((kCoordinatesPerVertex + kColorComponentsPerVertex) * mesh.get_faces().size());
    for (auto &face : mesh.get_faces()) {
      const glm::vec2 centroid = face->centroid();
      const glm::vec4 color = face->room_info ? face->room_info->color / 4.0f : glm::vec4(0.0f, 0.0f, 0.32f, 1.0f);
      drawable.data.insert(drawable.data.cend(), {
        centroid.x, centroid.y,
        color.r, color.g, color.b, color.a,
      });
    }
    drawable.element_count = static_cast<GLsizei>(mesh.get_faces().size());
    drawable.element_type = GL_POINTS;
    return drawable;
  }

  Drawable MeshEditor::SelectionBox() const {
    Drawable drawable;
    if (add_selecting || selecting) {
      constexpr size_t kVerticesPerEdge = 2;
      constexpr size_t kCoordinatesPerVertex = 2;
      constexpr size_t kColorComponentsPerVertex = 4;
      constexpr size_t kEdgeSize = kVerticesPerEdge * (kCoordinatesPerVertex + kColorComponentsPerVertex);
      const auto color = glm::vec4(0.64f, 0.64f, 0.0f, 1.0f);
      drawable.data.reserve(kEdgeSize * 4);
      const glm::vec2 cursor_end_position = get_cursor_position();
      drawable.data.insert(drawable.data.cend(), {
        cursor_start_position.x, cursor_start_position.y,
        color.r, color.g, color.b, color.a,
        cursor_end_position.x, cursor_start_position.y,
        color.r, color.g, color.b, color.a,
        cursor_start_position.x, cursor_start_position.y,
        color.r, color.g, color.b, color.a,
        cursor_start_position.x, cursor_end_position.y,
        color.r, color.g, color.b, color.a,
        cursor_end_position.x, cursor_end_position.y,
        color.r, color.g, color.b, color.a,
        cursor_start_position.x, cursor_end_position.y,
        color.r, color.g, color.b, color.a,
        cursor_end_position.x, cursor_end_position.y,
        color.r, color.g, color.b, color.a,
        cursor_end_position.x, cursor_start_position.y,
        color.r, color.g, color.b, color.a,
      });
      drawable.element_count = 8;
    } else {
      drawable.element_count = 0;
    }
    drawable.element_type = GL_LINES;
    return drawable;
  }

  void MeshEditor::Setup() {
    for (auto &half_edge : mesh.get_half_edges()) {
      if (half_edge->opposite) {
        CHECK_STATE(half_edge.get() == half_edge->opposite->opposite);
        if (half_edge->opposite->obstacle) {
          half_edge->obstacle = true;
        }
      }
    }
  }

  void MeshEditor::Update() {
    const bool ready = !(add_selecting || (MoveMode::kFalse != moving) || rotating ||
                         (ScaleMode::kFalse != scaling) || selecting);
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_1) > 0) {
      MeshSerializer serializer;
      serializer.WriteMesh(filename, mesh);
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_2) > 0) {
      selected_vertices.clear();
      MeshLoader loader;
      mesh = loader.ReadMesh(filename);
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_A) > 0) {
      if (selected_vertices.empty()) {
        for (auto &vertex : mesh.get_vertices()) {
          selected_vertices.insert(vertex.get());
        }
      } else {
        selected_vertices.clear();
      }
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_N) > 0 &&
        0 == mesh.get_vertices().size()) {
      selected_vertices.clear();
      mesh.AddDefaultFace(get_cursor_position());
      for (auto &vertex : mesh.get_vertices()) {
        selected_vertices.insert(vertex.get());
      }
      if (!selected_vertices.empty()) {
        for (auto vertex : selected_vertices) {
          selected_vertex_positions[vertex] = vertex->position;
        }
        moving = MoveMode::kBoth;
        cursor_start_position = get_cursor_position();
      }
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_D) > 0) {

      std::vector<Mesh::Vertex *> duplicate_vertices;
      std::vector<Mesh::HalfEdge *> duplicate_half_edges;
      std::vector<Mesh::Face *> duplicate_faces;

      std::unordered_map<Mesh::Vertex *, Mesh::Vertex *> vertex_map, duplicate_vertex_map;
      std::unordered_map<Mesh::HalfEdge *, Mesh::HalfEdge *> half_edge_map, duplicate_half_edge_map;
      std::unordered_map<Mesh::Face *, Mesh::Face *> face_map, duplicate_face_map;

      auto selected_faces = MeshEditor::selected_faces();
      for (auto face : selected_faces) {
        auto duplicate_face = new Mesh::Face;
        duplicate_faces.push_back(duplicate_face);
        mesh.get_faces().emplace_back(duplicate_face);
        face_map.insert({face, duplicate_face});
        duplicate_face_map.insert({duplicate_face, face});
        const auto h01 = face->face_edge;
        const auto h12 = h01->next;
        const auto h20 = h12->next;
        CHECK_STATE(h01 == h20->next);
        for (auto half_edge : {h01, h12, h20}) {
          auto duplicate_half_edge = new Mesh::HalfEdge;
          duplicate_half_edges.push_back(duplicate_half_edge);
          mesh.get_half_edges().emplace_back(duplicate_half_edge);
          half_edge_map.insert({half_edge, duplicate_half_edge});
          duplicate_half_edge_map.insert({duplicate_half_edge, half_edge});
        }
        const auto v0 = h01->start, v1 = h12->start, v2 = h20->start;
        for (auto vertex : {v0, v1, v2}) {
          if (vertex_map.end() == vertex_map.find(vertex)) {
            auto duplicate_vertex = new Mesh::Vertex;
            duplicate_vertices.push_back(duplicate_vertex);
            mesh.get_vertices().emplace_back(duplicate_vertex);
            vertex_map.insert({vertex, duplicate_vertex});
            duplicate_vertex_map.insert({duplicate_vertex, vertex});
          }
        }
      }

      for (auto duplicate_vertex : duplicate_vertices) {
        auto vertex = duplicate_vertex_map.at(duplicate_vertex);
        if (half_edge_map.end() != half_edge_map.find(vertex->vertex_edge)) {
          duplicate_vertex->vertex_edge = half_edge_map.at(vertex->vertex_edge);
        } else {
          duplicate_vertex->vertex_edge = nullptr;
        }
        duplicate_vertex->position = vertex->position;
      }
      for (auto duplicate_half_edge : duplicate_half_edges) {
        auto half_edge = duplicate_half_edge_map.at(duplicate_half_edge);
        if (face_map.end() != face_map.find(half_edge->face)) {
          duplicate_half_edge->face = face_map.at(half_edge->face);
        } else {
          duplicate_half_edge->face = nullptr;
        }
        if (half_edge_map.end() != half_edge_map.find(half_edge->next)) {
          duplicate_half_edge->next = half_edge_map.at(half_edge->next);
        } else {
          duplicate_half_edge->next = nullptr;
        }
        if (half_edge_map.end() != half_edge_map.find(half_edge->opposite)) {
          duplicate_half_edge->opposite = half_edge_map.at(half_edge->opposite);
        } else {
          duplicate_half_edge->opposite = nullptr;
        }
        if (half_edge_map.end() != half_edge_map.find(half_edge->previous)) {
          duplicate_half_edge->previous = half_edge_map.at(half_edge->previous);
        } else {
          duplicate_half_edge->previous = nullptr;
        }
        duplicate_half_edge->start = vertex_map.at(half_edge->start);
        duplicate_half_edge->generative = false;
      }
      for (auto duplicate_face : duplicate_faces) {
        auto face = duplicate_face_map.at(duplicate_face);
        duplicate_face->face_edge = half_edge_map.at(face->face_edge);
        duplicate_face->room_info = nullptr;
      }

      selected_vertices.clear();
      std::copy(duplicate_vertices.begin(), duplicate_vertices.end(),
                std::inserter(selected_vertices, selected_vertices.end()));
      if (!selected_vertices.empty()) {
        for (auto vertex : selected_vertices) {
          selected_vertex_positions[vertex] = vertex->position;
        }
        moving = MoveMode::kBoth;
        cursor_start_position = get_cursor_position();
      }
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_E) > 0) {
      std::unordered_set<Mesh::HalfEdge *> half_edges = selected_half_edges();
      if (half_edges.size() > 0) {
        selected_vertices.clear();
        for (auto &edge : half_edges) {
          if (!edge->opposite) {
            auto *vertex2 = mesh.ExtrudeEdge(edge);
            selected_vertices.insert(vertex2);
          }
        }
        for (auto vertex : selected_vertices) {
          selected_vertex_positions[vertex] = vertex->position;
        }
        moving = MoveMode::kBoth;
        cursor_start_position = get_cursor_position();
      }
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_F) > 0) {
      if (selected_vertices.size() == 3) {
        auto *vertex0 = *selected_vertices.begin();
        auto *vertex1 = *(std::next(selected_vertices.begin()));
        auto *vertex2 = *(std::next(selected_vertices.begin(), 2));
        mesh.AddFace(vertex0, vertex1, vertex2);
      }
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_X) > 0) {
      auto selected_faces = MeshEditor::selected_faces();
      std::unordered_set<Mesh::Face *> deleted_faces;
      std::unordered_set<Mesh::HalfEdge *> deleted_half_edges;
      std::unordered_set<Mesh::Vertex *> preserved_vertices;
      std::unordered_set<Mesh::Vertex *> deleted_vertices;
      std::copy(selected_faces.begin(), selected_faces.end(),
                std::inserter(deleted_faces, deleted_faces.end()));
      for (auto &face : deleted_faces) {
        auto h01 = face->face_edge;
        auto h12 = h01->next;
        auto h20 = h12->next;
        CHECK_STATE(h01 == h20->next);
        for (auto half_edge : {h01, h12, h20}) {
          deleted_half_edges.insert(half_edge);
        }
      }
      for (auto &half_edge : mesh.get_half_edges()) {
        if (deleted_half_edges.end() == deleted_half_edges.find(half_edge.get())) {
          preserved_vertices.insert(half_edge->start);
          if (deleted_half_edges.end() != deleted_half_edges.find(half_edge->opposite)) {
            half_edge->opposite = nullptr;
          }
        }
      }
      for (auto &vertex : mesh.get_vertices()) {
        if (preserved_vertices.end() == preserved_vertices.find(vertex.get())) {
          deleted_vertices.insert(vertex.get());
        }
      }
      for (auto vertex : preserved_vertices) {
        if (deleted_half_edges.end() != deleted_half_edges.find(vertex->vertex_edge)) {
          for (auto &half_edge : mesh.get_half_edges()) {
            if (deleted_half_edges.end() == deleted_half_edges.find(half_edge.get()) &&
                vertex == half_edge->start) {
              vertex->vertex_edge = half_edge.get();
              break;
            }
          }
        }
      }
      for (auto vertex : deleted_vertices) {
        selected_vertices.erase(vertex);
      }
      auto delete_vertex = [deleted_vertices] (std::unique_ptr<Mesh::Vertex> &vertex) {
        return deleted_vertices.end() != deleted_vertices.find(vertex.get());
      };
      auto delete_half_edge = [deleted_half_edges] (std::unique_ptr<Mesh::HalfEdge> &half_edge) {
        return deleted_half_edges.end() != deleted_half_edges.find(half_edge.get());
      };
      auto delete_face = [deleted_faces] (std::unique_ptr<Mesh::Face> &face) {
        return deleted_faces.end() != deleted_faces.find(face.get());
      };
      mesh.get_vertices().erase(std::remove_if(mesh.get_vertices().begin(),
                                               mesh.get_vertices().end(), delete_vertex),
                                mesh.get_vertices().end());
      mesh.get_half_edges().erase(std::remove_if(mesh.get_half_edges().begin(),
                                                 mesh.get_half_edges().end(), delete_half_edge),
                                mesh.get_half_edges().end());
      mesh.get_faces().erase(std::remove_if(mesh.get_faces().begin(),
                                            mesh.get_faces().end(), delete_face),
                                mesh.get_faces().end());
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_SPACE) > 0) {
      if (!selected_faces().empty()) {
        auto room_info = CreateRandomizedRoomInfo();
        mesh.get_room_infos().emplace_back(room_info);
        for (auto face : selected_faces()) {
          face->room_info = room_info;
        }
        selected_vertices.clear();
      }
    }
    if (ready && keyboard.GetKeyVelocity(GLFW_KEY_BACKSPACE) > 0) {
      for (auto face : selected_faces()) {
        face->room_info = nullptr;
      }
      selected_vertices.clear();
    }
    if (ready && !(keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT) ||
                   keyboard.IsKeyDown(GLFW_KEY_RIGHT_SHIFT)) &&
        mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) > 0) {
      selected_vertices.clear();
      selecting = true;
      cursor_start_position = get_cursor_position();
    }
    if (ready && (keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT) ||
                  keyboard.IsKeyDown(GLFW_KEY_RIGHT_SHIFT)) &&
        mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) > 0) {
      add_selecting = true;
      cursor_start_position = get_cursor_position();
    }
    if (ready && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_2) > 0) {
      Mesh::Vertex *closest_vertex = nullptr;
      double minimum_distance = std::numeric_limits<double>::max();
      const glm::vec2 cursor_position = get_cursor_position();
      for (auto &vertex : mesh.get_vertices()) {
        double distance = glm::length(vertex->position - cursor_position);
        if (distance < minimum_distance) {
          closest_vertex = vertex.get();
          minimum_distance = distance;
        }
      }
      if (closest_vertex) {
        if (!(keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT) ||
              keyboard.IsKeyDown(GLFW_KEY_RIGHT_SHIFT))) {
          selected_vertices.clear();
        }
        if (selected_vertices.end() == selected_vertices.find(closest_vertex)) {
          selected_vertices.insert(closest_vertex);
        } else {
          selected_vertices.erase(closest_vertex);
        }
      }
    }
    if (ready && !selected_vertices.empty() && keyboard.GetKeyVelocity(GLFW_KEY_G) > 0) {
      moving = MoveMode::kBoth;
      for (auto vertex : selected_vertices) {
        selected_vertex_positions[vertex] = vertex->position;
      }
      cursor_start_position = get_cursor_position();
    }
    if (ready && !selected_vertices.empty() && keyboard.GetKeyVelocity(GLFW_KEY_R) > 0) {
      rotating = true;
      for (auto vertex : selected_vertices) {
        selected_vertex_positions[vertex] = vertex->position;
      }
      cursor_start_position = get_cursor_position();
      center_of_mass = glm::vec2();
      float i = 1;
      for (auto vertex : selected_vertices) {
        center_of_mass += (vertex->position - center_of_mass) / i++;
      }
    }
    if (ready && !selected_half_edges().empty() && keyboard.GetKeyVelocity(GLFW_KEY_T) > 0) {
      for (auto half_edge : selected_half_edges()) {
        half_edge->obstacle = !half_edge->obstacle;
      }
    }
    if (ready && !selected_half_edges().empty() && keyboard.GetKeyVelocity(GLFW_KEY_Y) > 0) {
      for (auto half_edge : selected_half_edges()) {
        if (!half_edge->opposite) {
          half_edge->generative = true;
        }
      }
    }
    if (ready && !selected_vertices.empty() && keyboard.GetKeyVelocity(GLFW_KEY_S) > 0) {
      scaling = ScaleMode::kAll;
      for (auto vertex : selected_vertices) {
        selected_vertex_positions[vertex] = vertex->position;
      }
      cursor_start_position = get_cursor_position();
      center_of_mass = glm::vec2();
      float i = 1;
      for (auto vertex : selected_vertices) {
        center_of_mass += (vertex->position - center_of_mass) / i++;
      }
    }
    if (selecting && mouse.HasCursorMoved()) {
      selected_vertices.clear();
      const glm::vec2 cursor_end_position = get_cursor_position();
      const glm::vec2 top_left = glm::min(cursor_start_position, cursor_end_position);
      const glm::vec2 bottom_right = glm::max(cursor_start_position, cursor_end_position);
      for (auto &vertex : mesh.get_vertices()) {
        if (glm::all(glm::lessThan(top_left, vertex->position)) &&
            glm::all(glm::lessThan(vertex->position, bottom_right))) {
          selected_vertices.insert(vertex.get());
        }
      }
    }
    if (add_selecting && mouse.HasCursorMoved()) {
      additionally_selected_vertices.clear();
      const glm::vec2 cursor_end_position = get_cursor_position();
      const glm::vec2 top_left = glm::min(cursor_start_position, cursor_end_position);
      const glm::vec2 bottom_right = glm::max(cursor_start_position, cursor_end_position);
      for (auto &vertex : mesh.get_vertices()) {
        if (selected_vertices.end() == selected_vertices.find(vertex.get()) &&
            glm::all(glm::lessThan(top_left, vertex->position)) &&
            glm::all(glm::lessThan(vertex->position, bottom_right))) {
          additionally_selected_vertices.insert(vertex.get());
        }
      }
    }
    if (selecting && keyboard.GetKeyVelocity(GLFW_KEY_ESCAPE) > 0) {
      selecting = false;
      selected_vertices.clear();
    }
    if (selecting && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) < 0) {
      selecting = false;
    }
    if (add_selecting && keyboard.GetKeyVelocity(GLFW_KEY_ESCAPE) > 0) {
      add_selecting = false;
      additionally_selected_vertices.clear();
    }
    if (add_selecting && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) < 0) {
      add_selecting = false;
      std::copy(additionally_selected_vertices.begin(), additionally_selected_vertices.end(),
                std::inserter(selected_vertices, selected_vertices.end()));
      additionally_selected_vertices.clear();
    }
    if (MoveMode::kFalse != moving && mouse.HasCursorMoved()) {
      glm::vec2 d = get_cursor_position() - cursor_start_position;
      if (MoveMode::kX == moving) {
        d.y = 0;
      } else if (MoveMode::kY == moving) {
        d.x = 0;
      }
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex] + d;
      }
    }
    if (rotating && mouse.HasCursorMoved()) {
      const glm::vec2 cursor_end_position = get_cursor_position();
      const glm::vec2 start = cursor_start_position - center_of_mass;
      const glm::vec2 end = cursor_end_position - center_of_mass;
      const float start_angle = glm::atan(start.y, start.x);
      const float end_angle = glm::atan(end.y, end.x);
      const float angle = end_angle - start_angle;
      const glm::mat4 rotate = glm::rotate(glm::mat4(1.0), glm::degrees(angle), glm::vec3(0, 0, 1));
      for (auto vertex : selected_vertices) {
        const glm::vec4 r = rotate * glm::vec4(selected_vertex_positions[vertex] - center_of_mass,
                                               0.0, 1.0);
        vertex->position = center_of_mass + r.xy() / r.w;
      }
    }
    if (MoveMode::kFalse != moving && keyboard.GetKeyVelocity(GLFW_KEY_X) > 0) {
      moving = MoveMode::kX;
    }
    if (MoveMode::kFalse != moving && keyboard.GetKeyVelocity(GLFW_KEY_Y) > 0) {
      moving = MoveMode::kY;
    }
    if (MoveMode::kFalse != moving && keyboard.GetKeyVelocity(GLFW_KEY_B) > 0) {
      moving = MoveMode::kBoth;
    }
    if (ScaleMode::kFalse != scaling && keyboard.GetKeyVelocity(GLFW_KEY_X) > 0) {
      scaling = ScaleMode::kX;
    }
    if (ScaleMode::kFalse != scaling && keyboard.GetKeyVelocity(GLFW_KEY_Y) > 0) {
      scaling = ScaleMode::kY;
    }
    if (ScaleMode::kFalse != scaling && keyboard.GetKeyVelocity(GLFW_KEY_B) > 0) {
      scaling = ScaleMode::kBoth;
    }
    if (ScaleMode::kFalse != scaling && keyboard.GetKeyVelocity(GLFW_KEY_A) > 0) {
      scaling = ScaleMode::kAll;
    }
    if (ScaleMode::kFalse != scaling && mouse.HasCursorMoved()) {
      const glm::vec2 cursor_end_position = get_cursor_position();
      glm::vec2 scale = glm::vec2(1);
      if (ScaleMode::kAll == scaling) {
        const float s = glm::length(cursor_end_position - center_of_mass) / glm::length(
            cursor_start_position - center_of_mass);
        scale = glm::vec2(s);
      } else if (ScaleMode::kBoth == scaling) {
        scale = glm::abs(cursor_end_position - center_of_mass) / glm::abs(
            cursor_start_position - center_of_mass);
      } else if (ScaleMode::kX == scaling) {
        const float s = glm::abs(cursor_end_position.x - center_of_mass.x) / glm::abs(
            cursor_start_position.x - center_of_mass.x);
        scale = glm::vec2(s, 1);
      } else if (ScaleMode::kY == scaling) {
        const float s = glm::abs(cursor_end_position.y - center_of_mass.y) / glm::abs(
            cursor_start_position.y - center_of_mass.y);
        scale = glm::vec2(1, s);
      }
      for (auto vertex : selected_vertices) {
        vertex->position = center_of_mass + (
            scale * (selected_vertex_positions[vertex] - center_of_mass));
      }
    }
    if (MoveMode::kFalse != moving && keyboard.GetKeyVelocity(GLFW_KEY_ESCAPE) > 0) {
      moving = MoveMode::kFalse;
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex];
      }
    }
    if (rotating && keyboard.GetKeyVelocity(GLFW_KEY_ESCAPE) > 0) {
      rotating = false;
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex];
      }
    }
    if (ScaleMode::kFalse != scaling && keyboard.GetKeyVelocity(GLFW_KEY_ESCAPE) > 0) {
      scaling = ScaleMode::kFalse;
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex];
      }
    }
    if (MoveMode::kFalse != moving && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) > 0) {
      moving = MoveMode::kFalse;
      selected_vertex_positions.clear();
    }
    if (rotating && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) > 0) {
      rotating = false;
      selected_vertex_positions.clear();
    }
    if (ScaleMode::kFalse != scaling && mouse.GetButtonVelocity(GLFW_MOUSE_BUTTON_1) > 0) {
      scaling = ScaleMode::kFalse;
      selected_vertex_positions.clear();
    }
  }

}  // namespace textengine
