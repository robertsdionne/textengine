#include <Box2D/Box2D.h>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <limits>
#include <stb_vorbis.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "checks.h"
#include "gamestate.h"
#include "input.h"
#include "keyboard.h"
#include "log.h"
#include "scene.h"
#include "synchronizedqueue.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(SynchronizedQueue &reply_queue,
                   Log &playtest_log, Input &input, GameState &initial_state, Scene &scene)
  : reply_queue(reply_queue),
  playtest_log(playtest_log), input(input), current_state(initial_state),
  phrase_index(), scene(scene) {}

  void Updater::BeginContact(b2Contact *contact) {
    Object *area, *object;
    b2Body *player;
    std::tie(area, object, player) = ResolveContact(contact);
    if (player && area) {
      inside.insert(area);
      reply_queue.PushMessage(ChooseMessage(area->messages, "enter"));
    }
    const auto now = std::chrono::high_resolution_clock::now();
    if (player && object && now - last_touch_time[object] > std::chrono::seconds(2)) {
      last_touch_time[object] = now;
      reply_queue.PushMessage(ChooseMessage(object->messages, "touch"));
    }
  }

  void Updater::EndContact(b2Contact *contact) {
    Object *area, *object;
    b2Body *player;
    std::tie(area, object, player) = ResolveContact(contact);
    if (player && area) {
      reply_queue.PushMessage(ChooseMessage(area->messages, "exit"));
      inside.erase(area);
    }
  }

  std::tuple<Object *, Object *, b2Body *> Updater::ResolveContact(b2Contact *contact) const {
    Object *area = nullptr, *object = nullptr;
    b2Body *player = nullptr;
    if (current_state.player_body == contact->GetFixtureA()->GetBody()) {
      player = contact->GetFixtureA()->GetBody();
      if (contact->GetFixtureB()->IsSensor()) {
        area = reinterpret_cast<Object *>(contact->GetFixtureB()->GetBody()->GetUserData());
      } else {
        object = reinterpret_cast<Object *>(contact->GetFixtureB()->GetBody()->GetUserData());
      }
    }
    if (current_state.player_body == contact->GetFixtureB()->GetBody()) {
      player = contact->GetFixtureB()->GetBody();
      if (contact->GetFixtureA()->IsSensor()) {
        area = reinterpret_cast<Object *>(contact->GetFixtureA()->GetBody()->GetUserData());
      } else {
        object = reinterpret_cast<Object *>(contact->GetFixtureA()->GetBody()->GetUserData());
      }
    }
    return std::make_tuple(area, object, player);
  }

  std::string Updater::ChooseMessage(const MessageMap &messages, const std::string &name) {
    if (messages.at(name)->size()) {
      const auto index = index_distribution(generator) % messages.at(name)->size();
      return *messages.at(name)->at(index);
    } else {
      return "";
    }
  }

  bool Updater::Inside(const std::unique_ptr<Object> &area) const {
    return inside.cend() != inside.find(area.get());
  }

  GameState &Updater::GetCurrentState() {
    return current_state;
  }

  void Updater::Setup() {
    last_direction = Direction::kEast;
    current_state.world.SetContactListener(this);
  }

  void Updater::Update() {
    Update(current_state);
  }

  void Updater::Update(GameState &current_state) {
    const auto now = std::chrono::high_resolution_clock::now();
    const auto offset = input.GetPrimaryAxes();
    const auto offset2 = input.GetSecondaryAxes();

    auto dt = 0.016f;
    const auto position = glm::vec2(current_state.player_body->GetPosition().x,
                                    current_state.player_body->GetPosition().y);
    current_state.accrued_distance += glm::distance(position,
                                                    current_state.previous_player_position);
    current_state.previous_player_position = position;

    constexpr auto kStepSize = 0.5f;
    if (current_state.accrued_distance > kStepSize) {
      current_state.accrued_distance -= kStepSize;
      reply_queue.PushStep();
    }

    if (input.GetLookVelocity() > 0) {
      reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "look"));
      for (auto &area : scene.areas) {
        if (Inside(area)) {
          reply_queue.PushMessage(ChooseMessage(area->messages, "describe"));
        }
      }
    } else if (input.GetLookVelocity() < 0) {
      reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "stop looking"));
    }

    if (glm::length(offset) > 0.0 || input.GetTriggerVelocity() > 0.0) {
      current_state.world.ClearForces();
      auto velocity = current_state.player_body->GetLinearVelocity();

      last_direction_time = now;
      if (Direction::kEast != last_direction && offset.x > glm::abs(offset.y)) {
        last_direction = Direction::kEast;
        reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "east"));
      } else if (Direction::kWest != last_direction && offset.x < -glm::abs(offset.y)) {
        last_direction = Direction::kWest;
        reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "west"));
      } else if (Direction::kNorth != last_direction && offset.y > glm::abs(offset.x)) {
        last_direction = Direction::kNorth;
        reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "north"));
      } else if (Direction::kSouth != last_direction && offset.y < -glm::abs(offset.x)) {
        last_direction = Direction::kSouth;
        reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "south"));
      }

      const auto force = 200.0f * current_state.player_body->GetMass();
      if (input.GetTriggerVelocity() > 0) {
        reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "run"));
      } else if (input.GetTriggerVelocity() < 0) {
        reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "walk"));
      }
      const auto max_velocity = glm::mix(1.38f, 5.81f, input.GetTriggerPressure()) / 2.0f;
      current_state.player_body->ApplyForceToCenter(force * b2Vec2(offset.x, offset.y), true);
      auto target_angle = 0.0f;
      if (glm::length(offset2) > 0.1f) {
        target_angle = glm::atan(offset2.y, offset2.x);
      } else if (glm::length(offset) > 0.1f) {
        target_angle = glm::atan(offset.y, offset.x);
      }
      auto angle = current_state.player_body->GetAngle();
      while (target_angle - angle > M_PI) {
        target_angle -= 2.0f * M_PI;
      }
      while (target_angle - angle < -M_PI) {
        target_angle += 2.0f * M_PI;
      }
      angle = glm::mix(angle, target_angle, 0.25f);
      current_state.player_body->SetTransform(current_state.player_body->GetPosition(), angle);
      if (velocity.Length() > max_velocity) {
        velocity.Normalize();
        velocity *= max_velocity;
        current_state.player_body->SetLinearVelocity(velocity);
      }
      current_state.world.Step(dt, 8, 3);
    }
    current_state.camera_position = glm::mix(current_state.camera_position, position + 2.5f * offset2, 2e-2f / 0.016f * dt);
  }
  
}  // namespace textengine
