#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "drawable.h"
#include "gamestate.h"

namespace textengine {

  bool Item::operator ==(const Item &other) {
    return name == other.name;
  }

  GameState::GameState()
  : player{glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr},
  non_player_characters(), player_view_direction(glm::vec2(0, 1)),
  player_view_direction_target(glm::vec2(0, 1)), camera_position() {
    for (auto i = 0; i < 0; ++i) {
      non_player_characters.push_back({
        {glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr},
        {i}
      });
    }
  }

  GameState::GameState(glm::vec2 player_position,
                       glm::vec2 player_direction)
  : player{player_position, player_position, player_direction, player_direction, nullptr},
    non_player_characters() {
    for (auto i = 0; i < 0; ++i) {
      non_player_characters.push_back({
        {glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr},
        {i}
      });
    }
  }

  Drawable GameState::TriangulateItems() const {
    constexpr size_t kFacesPerItem = 2;
    constexpr size_t kVerticesPerFace = 3;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kItemSize = kFacesPerItem * kVerticesPerFace * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    constexpr float kSize = 0.01f;
    const auto p0 = glm::vec2(kSize, kSize), p1 = glm::vec2(-kSize, kSize),
        p2 = glm::vec2(-kSize, -kSize), p3 = glm::vec2(kSize, -kSize);
    Drawable drawable;
    drawable.data.reserve(kItemSize * items.size());
    for (auto &item : items) {
      drawable.data.insert(drawable.data.cend(), {
        (item.position + p0).x, (item.position + p0).y,
        item.color.r, item.color.g, item.color.b, item.color.a,
        (item.position + p1).x, (item.position + p1).y,
        item.color.r, item.color.g, item.color.b, item.color.a,
        (item.position + p2).x, (item.position + p2).y,
        item.color.r, item.color.g, item.color.b, item.color.a,
        (item.position + p0).x, (item.position + p0).y,
        item.color.r, item.color.g, item.color.b, item.color.a,
        (item.position + p2).x, (item.position + p2).y,
        item.color.r, item.color.g, item.color.b, item.color.a,
        (item.position + p3).x, (item.position + p3).y,
        item.color.r, item.color.g, item.color.b, item.color.a
      });
    }
    drawable.element_count = static_cast<GLsizei>(kFacesPerItem * kVerticesPerFace * items.size());
    drawable.element_type = GL_TRIANGLES;
    return drawable;
  }

  Drawable GameState::WireframeItems() const {
    constexpr size_t kEdgesPerItem = 4;
    constexpr size_t kVerticesPerEdge = 2;
    constexpr size_t kCoordinatesPerVertex = 2;
    constexpr size_t kColorComponentsPerVertex = 4;
    constexpr size_t kItemSize = kEdgesPerItem * kVerticesPerEdge * (kCoordinatesPerVertex + kColorComponentsPerVertex);
    constexpr float kSize = 0.01f;
    const auto p0 = glm::vec2(kSize, kSize), p1 = glm::vec2(-kSize, kSize),
        p2 = glm::vec2(-kSize, -kSize), p3 = glm::vec2(kSize, -kSize);
    Drawable drawable;
    drawable.data.reserve(kItemSize * items.size());
    for (auto &item : items) {
      const glm::vec4 color = item.color / 2.0f;
      drawable.data.insert(drawable.data.cend(), {
        (item.position + p0).x, (item.position + p0).y,
        color.r, color.g, color.b, color.a,
        (item.position + p1).x, (item.position + p1).y,
        color.r, color.g, color.b, color.a,
        (item.position + p1).x, (item.position + p1).y,
        color.r, color.g, color.b, color.a,
        (item.position + p2).x, (item.position + p2).y,
        color.r, color.g, color.b, color.a,
        (item.position + p2).x, (item.position + p2).y,
        color.r, color.g, color.b, color.a,
        (item.position + p3).x, (item.position + p3).y,
        color.r, color.g, color.b, color.a,
        (item.position + p3).x, (item.position + p3).y,
        color.r, color.g, color.b, color.a,
        (item.position + p0).x, (item.position + p0).y,
        color.r, color.g, color.b, color.a
      });
    }
    drawable.element_count = static_cast<GLsizei>(kEdgesPerItem * kVerticesPerEdge * items.size());
    drawable.element_type = GL_LINES;
    return drawable;
  }

}  // namespace textengine
