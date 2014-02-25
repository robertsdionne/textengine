#include <Box2D/Box2D.h>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <limits>
#include <map>
#include <stb_vorbis.h>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "checks.h"
#include "gamestate.h"
#include "input.h"
#include "keyboard.h"
#include "log.h"
#include "mouse.h"
#include "scene.h"
#include "synchronizedqueue.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(SynchronizedQueue &reply_queue,
                   Log &playtest_log, Input &input, Mouse &mouse, Keyboard &keyboard,
                   GameState &initial_state, Scene &scene)
  : reply_queue(reply_queue),
  playtest_log(playtest_log), input(input), mouse(mouse), keyboard(keyboard),
  current_state(initial_state), phrase_index(), scene(scene) {}

  void Updater::BeginContact(b2Contact *contact) {
    Object *area, *object;
    b2Body *player;
    std::tie(area, object, player) = ResolveContact(contact);
    if (player && area) {
      inside.insert(area);
      const auto enter = ChooseMessage(area->messages, "enter");
      if (!enter.empty()) {
        reply_queue.PushMessage(enter);
        reply_queue.PushEntity(area->id);
      }
    }
    const auto now = std::chrono::high_resolution_clock::now();
    if (player && object && now - last_touch_time[object] > std::chrono::seconds(2)) {
      last_touch_time[object] = now;
      const auto touch = ChooseMessage(object->messages, "touch");
      if (!touch.empty()) {
        reply_queue.PushMessage(touch);
        reply_queue.PushEntity(object->id);
      }
    }
  }

  void Updater::EndContact(b2Contact *contact) {
    Object *area, *object;
    b2Body *player;
    std::tie(area, object, player) = ResolveContact(contact);
    if (player && area) {
      const auto exit = ChooseMessage(area->messages, "exit");
      if (!exit.empty()) {
        reply_queue.PushMessage(exit);
        reply_queue.PushEntity(area->id);
      }
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
    if (HasMessage(messages, name)) {
      const auto index = index_distribution(generator) % messages.at(name)->size();
      return *messages.at(name)->at(index);
    } else {
      return "";
    }
  }
  
  bool Updater::HasMessage(const MessageMap &messages, const std::string &name) {
    return messages.cend() != messages.find(name) && messages.at(name)->size();
  }

  bool Updater::Inside(const std::unique_ptr<Object> &area) const {
    return inside.cend() != inside.find(area.get());
  }

  GameState &Updater::GetCurrentState() {
    return current_state;
  }
  
  void Updater::SetModelViewProjection(glm::mat4 model_view_projection) {}

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

    const auto dt = 0.016f;
    const auto position = glm::vec2(current_state.player_body->GetPosition().x,
                                    current_state.player_body->GetPosition().y);
    current_state.accrued_distance += glm::distance(position,
                                                    current_state.previous_player_position);
    current_state.previous_player_position = position;

    constexpr auto kStepSize = 1.0f;
    if (current_state.accrued_distance > kStepSize) {
      current_state.accrued_distance -= kStepSize;
      // reply_queue.PushStep();
    }
    
    if (now - last_transmit_time > std::chrono::milliseconds(16)) {
      auto directions = std::map<long, glm::vec2>();
      for (auto &object : scene.objects) {
        directions.insert({object->id, object->DirectionFrom(position)});
      }
      for (auto &area : scene.areas) {
        if (area->Contains(position)) {
          directions.insert({area->id, glm::vec2()});
        } else {
          directions.insert({area->id, area->DirectionFrom(position)});
        }
      }
      reply_queue.PushMovement(position,
                               glm::length(offset) > 0 ? glm::normalize(offset) : glm::vec2(),
                               directions);
      last_transmit_time = now;
    }

    if (input.GetLookVelocity() > 0) {
      std::ostringstream out;
      std::vector<Object *> nearby;
      for (auto &area : scene.areas) {
        if (Inside(area)) {
          out << ChooseMessage(area->messages, "inside") << " ";
        } else if (HasMessage(area->messages, "describe")) {
          nearby.push_back(area.get());
        }
      }
      for (auto &object : scene.objects) {
        if (HasMessage(object->messages, "describe")) {
          nearby.push_back(object.get());
        }
      }
      auto compare = [position] (const Object *a, const Object *b) {
        return a->DistanceTo(position) < b->DistanceTo(position);
      };
      auto nth = nearby.begin() + 3;
      std::partial_sort(nearby.begin(), nth, nearby.end(), compare);
      for (auto element = nearby.begin(); element < nth; ++element) {
        out << ChooseMessage((*element)->messages, "describe") << " ";
//        out << "(" << (*element)->DistanceTo(position) << ") ";
      }
      reply_queue.PushMessage(out.str());
    }

    if (glm::length(offset) > 0.0 || input.GetTriggerVelocity() > 0.0) {
      current_state.world.ClearForces();
      auto velocity = current_state.player_body->GetLinearVelocity();

      last_direction_time = now;
      // if (Direction::kEast != last_direction && offset.x > glm::abs(offset.y)) {
      //   last_direction = Direction::kEast;
      //   reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "east"));
      // } else if (Direction::kWest != last_direction && offset.x < -glm::abs(offset.y)) {
      //   last_direction = Direction::kWest;
      //   reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "west"));
      // } else if (Direction::kNorth != last_direction && offset.y > glm::abs(offset.x)) {
      //   last_direction = Direction::kNorth;
      //   reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "north"));
      // } else if (Direction::kSouth != last_direction && offset.y < -glm::abs(offset.x)) {
      //   last_direction = Direction::kSouth;
      //   reply_queue.PushMessage(ChooseMessage(scene.messages_by_name, "south"));
      // }

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
