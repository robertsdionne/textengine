#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "drawable.h"
#include "gamestate.h"

namespace textengine {

  bool Item::operator ==(const Item &other) {
    return name == other.name;
  }

  GameState::GameState(std::vector<std::unique_ptr<std::vector<glm::vec2>>> &&boundaries)
  : player{glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr},
  non_player_characters(), world(b2Vec2(0, 0)), player_view_direction(glm::vec2(0, 1)),
  player_view_direction_target(glm::vec2(0, 1)) {
    for (auto i = 0; i < 3; ++i) {
      non_player_characters.push_back({
        {glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr},
        {i}
      });
    }
      b2BodyDef boundary_body_definition;
      boundary_body_definition.position.Set(0, 0);
      boundary = world.CreateBody(&boundary_body_definition);
      for (auto &boundary : boundaries) {
        auto vertices = std::unique_ptr<b2Vec2[]>(new b2Vec2[boundary->size()]);
        for (auto i = 0; i < boundary->size(); ++i) {
          vertices[i].Set((*boundary)[i].x, (*boundary)[i].y);
        }
        b2ChainShape boundary_shape;
        boundary_shape.CreateLoop(vertices.get(), static_cast<int>(boundary->size()));
        b2FixtureDef boundary_fixture_definition;
        boundary_fixture_definition.shape = &boundary_shape;
        GameState::boundary->CreateFixture(&boundary_fixture_definition);
      }
      b2BodyDef player_body_definition;
      player_body_definition.type = b2_dynamicBody;
      player_body_definition.position.Set(player.position.x, player.position.y);
      player_body_definition.linearVelocity.Set(0, 0);
      player_body_definition.linearDamping = 2;
      player_body_definition.angularVelocity = 0;
      player_body_definition.angularDamping = 0.1;
      player_body = world.CreateBody(&player_body_definition);
      b2CircleShape player_shape;
      player_shape.m_radius = 0.001;
      b2FixtureDef player_fixture_definition;
      player_fixture_definition.shape = &player_shape;
      player_fixture_definition.density = 0.1;
      player_fixture_definition.restitution = 0.1;
      player_fixture_definition.friction = 0.1;
      player_body->CreateFixture(&player_fixture_definition);
  }

  GameState::GameState(glm::vec2 player_position,
                       glm::vec2 player_direction)
  : player{player_position, player_position, player_direction, player_direction, nullptr},
    non_player_characters(), world(b2Vec2(0, 0)) {
    for (auto i = 0; i < 3; ++i) {
      non_player_characters.push_back({
        {glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr},
        {i}
      });
    }
  }

  GameState::~GameState() {
    b2Fixture *fixture = boundary->GetFixtureList();
    while (fixture) {
      auto next = fixture->GetNext();
      boundary->DestroyFixture(fixture);
      fixture = next;
    }
    world.DestroyBody(boundary);
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
    drawable.data_size = kItemSize * items.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    int index = 0;
    for (auto &item : items) {
      drawable.data[index++] = (item.position + p0).x;
      drawable.data[index++] = (item.position + p0).y;
      drawable.data[index++] = item.color.r;
      drawable.data[index++] = item.color.g;
      drawable.data[index++] = item.color.b;
      drawable.data[index++] = item.color.a;
      drawable.data[index++] = (item.position + p1).x;
      drawable.data[index++] = (item.position + p1).y;
      drawable.data[index++] = item.color.r;
      drawable.data[index++] = item.color.g;
      drawable.data[index++] = item.color.b;
      drawable.data[index++] = item.color.a;
      drawable.data[index++] = (item.position + p2).x;
      drawable.data[index++] = (item.position + p2).y;
      drawable.data[index++] = item.color.r;
      drawable.data[index++] = item.color.g;
      drawable.data[index++] = item.color.b;
      drawable.data[index++] = item.color.a;
      drawable.data[index++] = (item.position + p0).x;
      drawable.data[index++] = (item.position + p0).y;
      drawable.data[index++] = item.color.r;
      drawable.data[index++] = item.color.g;
      drawable.data[index++] = item.color.b;
      drawable.data[index++] = item.color.a;
      drawable.data[index++] = (item.position + p2).x;
      drawable.data[index++] = (item.position + p2).y;
      drawable.data[index++] = item.color.r;
      drawable.data[index++] = item.color.g;
      drawable.data[index++] = item.color.b;
      drawable.data[index++] = item.color.a;
      drawable.data[index++] = (item.position + p3).x;
      drawable.data[index++] = (item.position + p3).y;
      drawable.data[index++] = item.color.r;
      drawable.data[index++] = item.color.g;
      drawable.data[index++] = item.color.b;
      drawable.data[index++] = item.color.a;
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
    drawable.data_size = kItemSize * items.size();
    drawable.data = std::unique_ptr<float[]>{new float[drawable.data_size]};
    int index = 0;
    for (auto &item : items) {
      const glm::vec4 color = item.color / 2.0f;
      drawable.data[index++] = (item.position + p0).x;
      drawable.data[index++] = (item.position + p0).y;
      drawable.data[index++] = color.r;
      drawable.data[index++] = color.g;
      drawable.data[index++] = color.b;
      drawable.data[index++] = color.a;
      drawable.data[index++] = (item.position + p1).x;
      drawable.data[index++] = (item.position + p1).y;
      drawable.data[index++] = color.r;
      drawable.data[index++] = color.g;
      drawable.data[index++] = color.b;
      drawable.data[index++] = color.a;
      drawable.data[index++] = (item.position + p1).x;
      drawable.data[index++] = (item.position + p1).y;
      drawable.data[index++] = color.r;
      drawable.data[index++] = color.g;
      drawable.data[index++] = color.b;
      drawable.data[index++] = color.a;
      drawable.data[index++] = (item.position + p2).x;
      drawable.data[index++] = (item.position + p2).y;
      drawable.data[index++] = color.r;
      drawable.data[index++] = color.g;
      drawable.data[index++] = color.b;
      drawable.data[index++] = color.a;
      drawable.data[index++] = (item.position + p2).x;
      drawable.data[index++] = (item.position + p2).y;
      drawable.data[index++] = color.r;
      drawable.data[index++] = color.g;
      drawable.data[index++] = color.b;
      drawable.data[index++] = color.a;
      drawable.data[index++] = (item.position + p3).x;
      drawable.data[index++] = (item.position + p3).y;
      drawable.data[index++] = color.r;
      drawable.data[index++] = color.g;
      drawable.data[index++] = color.b;
      drawable.data[index++] = color.a;
      drawable.data[index++] = (item.position + p3).x;
      drawable.data[index++] = (item.position + p3).y;
      drawable.data[index++] = color.r;
      drawable.data[index++] = color.g;
      drawable.data[index++] = color.b;
      drawable.data[index++] = color.a;
      drawable.data[index++] = (item.position + p0).x;
      drawable.data[index++] = (item.position + p0).y;
      drawable.data[index++] = color.r;
      drawable.data[index++] = color.g;
      drawable.data[index++] = color.b;
      drawable.data[index++] = color.a;
    }
    drawable.element_count = static_cast<GLsizei>(kEdgesPerItem * kVerticesPerEdge * items.size());
    drawable.element_type = GL_LINES;
    return drawable;
  }

}  // namespace textengine
