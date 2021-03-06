#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include <memory>
#include <random>

#include "drawable.h"
#include "gamestate.h"
#include "scene.h"

namespace textengine {

  GameState::GameState(Scene &scene)
  : camera_position(), previous_player_position(), accrued_distance(), zoom(1.0),
    world(b2Vec2(0.0f, 0.0f)), player_body(), selected_item() {
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

    for (const auto &area : scene.areas) {
      b2BodyDef area_body_definition;
      area_body_definition.position.Set(area->aabb.center().x, area->aabb.center().y);
      area_body_definition.fixedRotation = true;
      area_body_definition.userData = area.get();
      areas.push_back(world.CreateBody(&area_body_definition));
      b2PolygonShape aabb_shape;
      b2CircleShape circle_shape;
      b2FixtureDef area_fixture_definition;
      if (Shape::kAxisAlignedBoundingBox == area->shape) {
        aabb_shape.SetAsBox(area->aabb.half_extent().x, area->aabb.half_extent().y);
        area_fixture_definition.shape = &aabb_shape;
      } else {
        circle_shape.m_radius = area->aabb.radius();
        area_fixture_definition.shape = &circle_shape;
      }
      area_fixture_definition.isSensor = true;
      areas.back()->CreateFixture(&area_fixture_definition);
    }
    for (const auto &object : scene.objects) {
      b2BodyDef object_body_definition;
      object_body_definition.position.Set(object->aabb.center().x, object->aabb.center().y);
      object_body_definition.fixedRotation = true;
      object_body_definition.userData = object.get();
      objects.push_back(world.CreateBody(&object_body_definition));
      b2PolygonShape aabb_shape;
      b2CircleShape circle_shape;
      b2FixtureDef object_fixture_definition;
      if (Shape::kAxisAlignedBoundingBox == object->shape) {
        aabb_shape.SetAsBox(object->aabb.half_extent().x, object->aabb.half_extent().y);
        object_fixture_definition.shape = &aabb_shape;
      } else {
        circle_shape.m_radius = object->aabb.radius();
        object_fixture_definition.shape = &circle_shape;
      }
      object_fixture_definition.friction = 0.5f;
      objects.back()->CreateFixture(&object_fixture_definition);
    }
  }

  GameState::~GameState() {
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
