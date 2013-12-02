#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include <memory>
#include <random>

#include "drawable.h"
#include "gamestate.h"
#include "mesh.h"

namespace textengine {

  GameState::GameState(std::vector<std::unique_ptr<std::vector<glm::vec2>>> &&boundaries, Mesh &mesh)
  : camera_position(), world(b2Vec2(0.0f, 0.0f)), boundary(), player_body(), target_angle(),
  flashlight_on() {
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
      boundary_fixture_definition.friction = 1.0f;
      GameState::boundary->CreateFixture(&boundary_fixture_definition);
    }
    b2BodyDef player_body_definition;
    player_body_definition.type = b2_dynamicBody;
    player_body_definition.position.Set(0.0f, 0.0f);
    player_body_definition.fixedRotation = true;
    player_body_definition.linearVelocity.Set(0, 0);
    player_body_definition.linearDamping = 8;
    player_body_definition.angularVelocity = 0;
    player_body_definition.angularDamping = 1;
    player_body = world.CreateBody(&player_body_definition);
    b2CircleShape player_shape;
    player_shape.m_radius = 0.01;
    b2FixtureDef player_fixture_definition;
    player_fixture_definition.shape = &player_shape;
    player_fixture_definition.density = 0.1;
    player_fixture_definition.restitution = 0.1;
    player_fixture_definition.friction = 1.0f;
    player_body->CreateFixture(&player_fixture_definition);

    b2BodyDef rat_body_definition;
    rat_body_definition.type = b2_dynamicBody;
    rat_body_definition.position.Set(0.0f, 0.0f);
    rat_body_definition.fixedRotation = true;
    rat_body_definition.linearVelocity.Set(0, 0.1);
    rat_body_definition.linearDamping = 0.1;
    rat_body_definition.angularVelocity = 0;
    rat_body_definition.angularDamping = 1;
    std::random_device generator;
    std::uniform_int_distribution<> distribution;
    for (auto i = 0; i < 500; ++i) {
      auto index = distribution(generator);
      auto centroid = mesh.get_faces()[index % mesh.get_faces().size()]->centroid();
      rat_body_definition.position.Set(centroid.x, centroid.y);
      rats.push_back(world.CreateBody(&rat_body_definition));
      rats.back()->CreateFixture(&player_fixture_definition);
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
    boundary = nullptr;
  }

  Drawable GameState::Shots() const {
    Drawable drawable;
    drawable.data.reserve(shots.size());
    for (auto &shot : shots) {
      const auto amount = 0.1f * shot.intensity;
      drawable.data.insert(drawable.data.cend(), {
//        shot.start.x, shot.start.y,
//        1.0f, 1.0f, 1.0f, amount,
        shot.end.x, shot.end.y,
        1.0f, 1.0f, 1.0f, amount
      });
    }
    drawable.element_count = static_cast<GLsizei>(shots.size());
    drawable.element_type = GL_POINTS;
    return drawable;
  }

}  // namespace textengine
