#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include <memory>
#include <random>

#include "drawable.h"
#include "gamestate.h"
#include "scene.h"

namespace textengine {

  GameState::GameState(Scene &scene,
                       std::vector<std::unique_ptr<std::vector<glm::vec2>>> &&boundaries)
  : camera_position(), world(b2Vec2(0.0f, 0.0f)), boundary(), player_body() {
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
      boundary_fixture_definition.friction = 0.0f;
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
    player_shape.m_radius = 0.125f;
    b2FixtureDef player_fixture_definition;
    player_fixture_definition.shape = &player_shape;
    player_fixture_definition.density = 0.1;
    player_fixture_definition.restitution = 0.1;
    player_fixture_definition.friction = 0.5f;
    player_body->CreateFixture(&player_fixture_definition);

    for (auto &area : scene.areas) {
      b2BodyDef area_body_definition;
      area_body_definition.position.Set(area->aabb.center().x, area->aabb.center().y);
      area_body_definition.fixedRotation = true;
      area_body_definition.userData = area.get();
      areas.push_back(world.CreateBody(&area_body_definition));
      b2PolygonShape area_shape;
      area_shape.SetAsBox(area->aabb.extent().x / 2.0f, area->aabb.extent().y / 2.0f);
      b2FixtureDef area_fixture_definition;
      area_fixture_definition.shape = &area_shape;
      area_fixture_definition.isSensor = true;
      areas.back()->CreateFixture(&area_fixture_definition);
    }
    for (auto &object : scene.objects) {
      b2BodyDef object_body_definition;
      object_body_definition.position.Set(object->position.x, object->position.y);
      object_body_definition.fixedRotation = true;
      object_body_definition.userData = object.get();
      objects.push_back(world.CreateBody(&object_body_definition));
      b2CircleShape object_shape;
      object_shape.m_radius = 0.5f;
      b2FixtureDef object_fixture_definition;
      object_fixture_definition.shape = &object_shape;
      object_fixture_definition.friction = 0.5f;
      objects.back()->CreateFixture(&object_fixture_definition);
    }
  }

  GameState::~GameState() {
    if (boundary) {
      world.DestroyBody(boundary);
      boundary = nullptr;
    }
    // TODO(robertsdionne): debug the mutex lock failure; not even sure why that's happening.
//    if (player_body) {
//      world.DestroyBody(player_body);
//      player_body = nullptr;
//    }
//    for (auto &area : areas) {
//      if (area) {
//        world.DestroyBody(area);
//        area = nullptr;
//      }
//    }
    for (auto &object : objects) {
      if (object) {
        world.DestroyBody(object);
        object = nullptr;
      }
    }
  }

}  // namespace textengine
