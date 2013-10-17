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
                         std::default_random_engine &engine)
  : width(width), height(height), keyboard(keyboard), mouse(mouse), mesh(mesh), selected_vertices(),
    selected_vertex_positions(), cursor_start_position(), engine(engine), uniform_real(),
    uniform_int(0, 26) {}

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

  glm::vec2 MeshEditor::FaceCentroid(const Mesh::Face *face) const {
    glm::vec2 total = glm::vec2();
    float count = 0;
    Mesh::HalfEdge *edge = face->face_edge;
    do {
      total += edge->start->position;
      count += 1;
      edge = edge->next;
    } while (edge != face->face_edge);
    return total / count;
  }

  Drawable MeshEditor::HighlightedPoints() const {
    std::unordered_set<Mesh::Vertex *> vertices = potentially_selected_vertices();
    Drawable drawable;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    const auto color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    drawable.data_size = (kCoordinatesPerVertex + kColorComponentsPerVertex) * vertices.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    auto vertex = vertices.begin();
    for (auto i = 0; i < vertices.size(); ++i, ++vertex) {
      drawable.data[(kCoordinatesPerVertex + kColorComponentsPerVertex) * i + 0] = (*vertex)->position.x;
      drawable.data[(kCoordinatesPerVertex + kColorComponentsPerVertex) * i + 1] = (*vertex)->position.y;
      drawable.data[(kCoordinatesPerVertex + kColorComponentsPerVertex) * i + 2] = color.r;
      drawable.data[(kCoordinatesPerVertex + kColorComponentsPerVertex) * i + 3] = color.g;
      drawable.data[(kCoordinatesPerVertex + kColorComponentsPerVertex) * i + 4] = color.b;
      drawable.data[(kCoordinatesPerVertex + kColorComponentsPerVertex) * i + 5] = color.a;
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
    drawable.data_size = kFaceSize * faces.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    auto face = faces.begin();
    const auto color = glm::vec4(0.32f, 0.0f, 0.0f, 1.0f);
    for (auto i = 0; i < faces.size(); ++i, ++face) {
      const auto h01 = (*face)->face_edge;
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
    drawable.data_size = kEdgeSize * interior_edges;
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    int index = 0;
    for (auto &half_edge : half_edges) {
      if (half_edge->opposite) {
        drawable.data[index + 0] = half_edge->start->position.x;
        drawable.data[index + 1] = half_edge->start->position.y;
        drawable.data[index + 2] = color.r;
        drawable.data[index + 3] = color.g;
        drawable.data[index + 4] = color.b;
        drawable.data[index + 5] = color.a;
        drawable.data[index + 6] = half_edge->next->start->position.x;
        drawable.data[index + 7] = half_edge->next->start->position.y;
        drawable.data[index + 8] = color.r;
        drawable.data[index + 9] = color.g;
        drawable.data[index + 10] = color.b;
        drawable.data[index + 11] = color.a;
        index += kEdgeSize;
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
    drawable.data_size = kEdgeSize * exterior_edges;
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    int index = 0;
    for (auto &half_edge : half_edges) {
      if (!half_edge->opposite) {
        drawable.data[index + 0] = half_edge->start->position.x;
        drawable.data[index + 1] = half_edge->start->position.y;
        drawable.data[index + 2] = color.r;
        drawable.data[index + 3] = color.g;
        drawable.data[index + 4] = color.b;
        drawable.data[index + 5] = color.a;
        drawable.data[index + 6] = half_edge->next->start->position.x;
        drawable.data[index + 7] = half_edge->next->start->position.y;
        drawable.data[index + 8] = color.r;
        drawable.data[index + 9] = color.g;
        drawable.data[index + 10] = color.b;
        drawable.data[index + 11] = color.a;
        index += kEdgeSize;
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
      drawable.data_size = kEdgeSize;
      drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
      const glm::vec2 cursor_end_position = get_cursor_position();
      drawable.data[0] = cursor_start_position.x;
      drawable.data[1] = cursor_start_position.y;
      drawable.data[2] = color.r;
      drawable.data[3] = color.g;
      drawable.data[4] = color.b;
      drawable.data[5] = color.a;
      if (MoveMode::kBoth == moving) {
        drawable.data[6] = cursor_end_position.x;
        drawable.data[7] = cursor_end_position.y;
      } else if (MoveMode::kX == moving) {
        drawable.data[6] = cursor_end_position.x;
        drawable.data[7] = cursor_start_position.y;
      } else if (MoveMode::kY == moving) {
        drawable.data[6] = cursor_start_position.x;
        drawable.data[7] = cursor_end_position.y;
      }
      drawable.data[8] = color.r;
      drawable.data[9] = color.g;
      drawable.data[10] = color.b;
      drawable.data[11] = color.a;
      drawable.element_count = 2;
    } else if (rotating) {
      drawable.data_size = kEdgeSize * 2;
      drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
      const glm::vec2 cursor_end_position = get_cursor_position();
      drawable.data[0] = center_of_mass.x;
      drawable.data[1] = center_of_mass.y;
      drawable.data[2] = color.r;
      drawable.data[3] = color.g;
      drawable.data[4] = color.b;
      drawable.data[5] = color.a;
      drawable.data[6] = cursor_start_position.x;
      drawable.data[7] = cursor_start_position.y;
      drawable.data[8] = color.r;
      drawable.data[9] = color.g;
      drawable.data[10] = color.b;
      drawable.data[11] = color.a;
      drawable.data[12] = center_of_mass.x;
      drawable.data[13] = center_of_mass.y;
      drawable.data[14] = color.r;
      drawable.data[15] = color.g;
      drawable.data[16] = color.b;
      drawable.data[17] = color.a;
      drawable.data[18] = cursor_end_position.x;
      drawable.data[19] = cursor_end_position.y;
      drawable.data[20] = color.r;
      drawable.data[21] = color.g;
      drawable.data[22] = color.b;
      drawable.data[23] = color.a;
      drawable.element_count = 4;
    } else if (ScaleMode::kFalse != scaling) {
      drawable.data_size = kEdgeSize;
      drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
      const glm::vec2 cursor_end_position = get_cursor_position();
      drawable.data[0] = center_of_mass.x;
      drawable.data[1] = center_of_mass.y;
      drawable.data[2] = color.r;
      drawable.data[3] = color.g;
      drawable.data[4] = color.b;
      drawable.data[5] = color.a;
      if (ScaleMode::kAll == scaling || ScaleMode::kBoth == scaling) {
        drawable.data[6] = cursor_end_position.x;
        drawable.data[7] = cursor_end_position.y;
      } else if (ScaleMode::kX == scaling) {
        drawable.data[6] = cursor_end_position.x;
        drawable.data[7] = center_of_mass.y;
      } else if (ScaleMode::kY == scaling) {
        drawable.data[6] = center_of_mass.x;
        drawable.data[7] = cursor_end_position.y;
      }
      drawable.data[8] = color.r;
      drawable.data[9] = color.g;
      drawable.data[10] = color.b;
      drawable.data[11] = color.a;
      drawable.element_count = 2;
    } else {
      drawable.data_size = 0;
      drawable.data = std::unique_ptr<float[]>();
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
    drawable.data_size = kEdgeSize * interior_edges;
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    int index = 0;
    for (auto &half_edge : mesh.get_half_edges()) {
      if (half_edge->opposite) {
        const glm::vec2 centroid = FaceCentroid(half_edge->face);
        const glm::vec2 opposite_centroid = FaceCentroid(half_edge->opposite->face);
        const glm::vec4 color = half_edge->face->room_info ? half_edge->face->room_info->color / 2.0f : glm::vec4(0.0f, 0.0f, 0.64f, 1.0f);
        const glm::vec4 opposite_color = half_edge->opposite->face->room_info ? half_edge->opposite->face->room_info->color / 2.0f : glm::vec4(0.0f, 0.0f, 0.64f, 1.0f);
        drawable.data[index + 0] = centroid.x;
        drawable.data[index + 1] = centroid.y;
        drawable.data[index + 2] = color.r;
        drawable.data[index + 3] = color.g;
        drawable.data[index + 4] = color.b;
        drawable.data[index + 5] = color.a;
        drawable.data[index + 6] = opposite_centroid.x;
        drawable.data[index + 7] = opposite_centroid.y;
        drawable.data[index + 8] = opposite_color.r;
        drawable.data[index + 9] = opposite_color.g;
        drawable.data[index + 10] = opposite_color.b;
        drawable.data[index + 11] = opposite_color.a;
        index += kEdgeSize;
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
    drawable.data_size = (kCoordinatesPerVertex + kColorComponentsPerVertex) * mesh.get_faces().size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    int index = 0;
    for (auto &face : mesh.get_faces()) {
      const glm::vec2 centroid = FaceCentroid(face.get());
      const glm::vec4 color = face->room_info ? face->room_info->color / 4.0f : glm::vec4(0.0f, 0.0f, 0.32f, 1.0f);
      drawable.data[index + 0] = centroid.x;
      drawable.data[index + 1] = centroid.y;
      drawable.data[index + 2] = color.r;
      drawable.data[index + 3] = color.g;
      drawable.data[index + 4] = color.b;
      drawable.data[index + 5] = color.a;
      index += kCoordinatesPerVertex + kColorComponentsPerVertex;
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
      drawable.data_size = kEdgeSize * 4;
      drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
      const glm::vec2 cursor_end_position = get_cursor_position();
      drawable.data[0] = cursor_start_position.x;
      drawable.data[1] = cursor_start_position.y;
      drawable.data[2] = color.r;
      drawable.data[3] = color.g;
      drawable.data[4] = color.b;
      drawable.data[5] = color.a;
      drawable.data[6] = cursor_end_position.x;
      drawable.data[7] = cursor_start_position.y;
      drawable.data[8] = color.r;
      drawable.data[9] = color.g;
      drawable.data[10] = color.b;
      drawable.data[11] = color.a;
      drawable.data[12] = cursor_start_position.x;
      drawable.data[13] = cursor_start_position.y;
      drawable.data[14] = color.r;
      drawable.data[15] = color.g;
      drawable.data[16] = color.b;
      drawable.data[17] = color.a;
      drawable.data[18] = cursor_start_position.x;
      drawable.data[19] = cursor_end_position.y;
      drawable.data[20] = color.r;
      drawable.data[21] = color.g;
      drawable.data[22] = color.b;
      drawable.data[23] = color.a;
      drawable.data[24] = cursor_end_position.x;
      drawable.data[25] = cursor_end_position.y;
      drawable.data[26] = color.r;
      drawable.data[27] = color.g;
      drawable.data[28] = color.b;
      drawable.data[29] = color.a;
      drawable.data[30] = cursor_start_position.x;
      drawable.data[31] = cursor_end_position.y;
      drawable.data[32] = color.r;
      drawable.data[33] = color.g;
      drawable.data[34] = color.b;
      drawable.data[35] = color.a;
      drawable.data[36] = cursor_end_position.x;
      drawable.data[37] = cursor_end_position.y;
      drawable.data[38] = color.r;
      drawable.data[39] = color.g;
      drawable.data[40] = color.b;
      drawable.data[41] = color.a;
      drawable.data[42] = cursor_end_position.x;
      drawable.data[43] = cursor_start_position.y;
      drawable.data[44] = color.r;
      drawable.data[45] = color.g;
      drawable.data[46] = color.b;
      drawable.data[47] = color.a;
      drawable.element_count = 8;
    } else {
      drawable.data_size = 0;
      drawable.data = std::unique_ptr<float[]>();
      drawable.element_count = 0;
    }
    drawable.element_type = GL_LINES;
    return drawable;
  }

  void MeshEditor::Update() {
    const bool ready = !(add_selecting || (MoveMode::kFalse != moving) || rotating ||
                         (ScaleMode::kFalse != scaling) || selecting);
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_1)) {
      MeshSerializer serializer;
      serializer.WriteMesh("../resource/output.json", mesh);
    }
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_2)) {
      selected_vertices.clear();
      MeshLoader loader;
      mesh = loader.ReadMesh("../resource/output.json");
    }
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_A)) {
      if (selected_vertices.empty()) {
        for (auto &vertex : mesh.get_vertices()) {
          selected_vertices.insert(vertex.get());
        }
      } else {
        selected_vertices.clear();
      }
    }
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_D)) {

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
      }
      for (auto duplicate_face : duplicate_faces) {
        auto face = duplicate_face_map.at(duplicate_face);
        duplicate_face->face_edge = half_edge_map.at(face->face_edge);
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
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_E)) {
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
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_F)) {
      if (selected_vertices.size() == 3) {
        auto *vertex0 = *selected_vertices.begin();
        auto *vertex1 = *(std::next(selected_vertices.begin()));
        auto *vertex2 = *(std::next(selected_vertices.begin(), 2));
        mesh.AddFace(vertex0, vertex1, vertex2);
      }
    }
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_X)) {
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
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_SPACE)) {
      if (!selected_faces().empty()) {
        auto room_info = CreateRandomizedRoomInfo();
        std::cout << "Created room: " << room_info->name << std::endl;
        std::cout << "  with color: " << room_info->color.r << ", " << room_info->color.g << ", "
        << room_info->color.b << std::endl;
        mesh.get_room_infos().emplace_back(room_info);
        for (auto face : selected_faces()) {
          face->room_info = room_info;
        }
        selected_vertices.clear();
      }
    }
    if (ready && keyboard.IsKeyJustPressed(GLFW_KEY_BACKSPACE)) {
      for (auto face : selected_faces()) {
        face->room_info = nullptr;
      }
      selected_vertices.clear();
    }
    if (ready && !(keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT) ||
                   keyboard.IsKeyDown(GLFW_KEY_RIGHT_SHIFT)) &&
        mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      selected_vertices.clear();
      selecting = true;
      cursor_start_position = get_cursor_position();
    }
    if (ready && (keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT) ||
                  keyboard.IsKeyDown(GLFW_KEY_RIGHT_SHIFT)) &&
        mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      add_selecting = true;
      cursor_start_position = get_cursor_position();
    }
    if (ready && mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_2)) {
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
    if (ready && !selected_vertices.empty() && keyboard.IsKeyJustPressed(GLFW_KEY_G)) {
      moving = MoveMode::kBoth;
      for (auto vertex : selected_vertices) {
        selected_vertex_positions[vertex] = vertex->position;
      }
      cursor_start_position = get_cursor_position();
    }
    if (ready && !selected_vertices.empty() && keyboard.IsKeyJustPressed(GLFW_KEY_R)) {
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
    if (ready && !selected_vertices.empty() && keyboard.IsKeyJustPressed(GLFW_KEY_S)) {
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
    if (selecting && keyboard.IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
      selecting = false;
      selected_vertices.clear();
    }
    if (selecting && mouse.IsButtonJustReleased(GLFW_MOUSE_BUTTON_1)) {
      selecting = false;
    }
    if (add_selecting && keyboard.IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
      add_selecting = false;
      additionally_selected_vertices.clear();
    }
    if (add_selecting && mouse.IsButtonJustReleased(GLFW_MOUSE_BUTTON_1)) {
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
    if (MoveMode::kFalse != moving && keyboard.IsKeyJustPressed(GLFW_KEY_X)) {
      moving = MoveMode::kX;
    }
    if (MoveMode::kFalse != moving && keyboard.IsKeyJustPressed(GLFW_KEY_Y)) {
      moving = MoveMode::kY;
    }
    if (MoveMode::kFalse != moving && keyboard.IsKeyJustPressed(GLFW_KEY_B)) {
      moving = MoveMode::kBoth;
    }
    if (ScaleMode::kFalse != scaling && keyboard.IsKeyJustPressed(GLFW_KEY_X)) {
      scaling = ScaleMode::kX;
    }
    if (ScaleMode::kFalse != scaling && keyboard.IsKeyJustPressed(GLFW_KEY_Y)) {
      scaling = ScaleMode::kY;
    }
    if (ScaleMode::kFalse != scaling && keyboard.IsKeyJustPressed(GLFW_KEY_B)) {
      scaling = ScaleMode::kBoth;
    }
    if (ScaleMode::kFalse != scaling && keyboard.IsKeyJustPressed(GLFW_KEY_A)) {
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
    if (MoveMode::kFalse != moving && keyboard.IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
      moving = MoveMode::kFalse;
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex];
      }
    }
    if (rotating && keyboard.IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
      rotating = false;
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex];
      }
    }
    if (ScaleMode::kFalse != scaling && keyboard.IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
      scaling = ScaleMode::kFalse;
      for (auto vertex : selected_vertices) {
        vertex->position = selected_vertex_positions[vertex];
      }
    }
    if (MoveMode::kFalse != moving && mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      moving = MoveMode::kFalse;
      selected_vertex_positions.clear();
    }
    if (rotating && mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      rotating = false;
      selected_vertex_positions.clear();
    }
    if (ScaleMode::kFalse != scaling && mouse.IsButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      scaling = ScaleMode::kFalse;
      selected_vertex_positions.clear();
    }
  }

}  // namespace textengine
