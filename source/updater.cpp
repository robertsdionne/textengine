#include <Box2D/Box2D.h>
#include <cmath>
#include <glm/glm.hpp>
#include <limits>
#include <string>
#include <unordered_map>

#include "checks.h"
#include "commandparser.h"
#include "gamestate.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "mesh.h"
#include "synchronizedqueue.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(SynchronizedQueue &command_queue, SynchronizedQueue &reply_queue,
                   Log &playtest_log, CommandParser &parser, Joystick &joystick,
                   Mesh &mesh, GameState &initial_state)
  : command_queue(command_queue), reply_queue(reply_queue), playtest_log(playtest_log),
    parser(parser), joystick(joystick), mesh(mesh), current_state(initial_state), clock(),
    last_approach_times(), phrase_index() {}

  GameState &Updater::GetCurrentState() {
    return current_state;
  }

  void Updater::Update() {
    Update(current_state);
  }

  void Updater::Update(GameState &current_state) {
    if (command_queue.HasMessage()) {
      const std::string message = command_queue.PopMessage().message;
      playtest_log.LogMessage(message);
      parser.Parse(current_state, message);
    }
    auto offset = glm::vec2(joystick.GetAxis(Joystick::Axis::kLeftX),
                            -joystick.GetAxis(Joystick::Axis::kLeftY));
    auto offset2 = glm::vec2(joystick.GetAxis(Joystick::Axis::kRightX),
                             -joystick.GetAxis(Joystick::Axis::kRightY));
    auto current_velocity = glm::vec2(current_state.player_body->GetLinearVelocity().x,
                                      current_state.player_body->GetLinearVelocity().y);
    if (glm::length(offset) > 0 || glm::length(offset2) > 0 ||
        joystick.GetButtonPressure(Joystick::PressureButton::kX) > 0) {
      auto position = glm::vec2(current_state.player_body->GetPosition().x,
                                current_state.player_body->GetPosition().y);
      auto current_face = FindFaceThatContainsPoint(position);
      float maximum = -std::numeric_limits<float>::infinity();
      glm::vec2 target;
      Mesh::Face *argmax = nullptr;
      current_face->ForEachHalfEdge([&] (Mesh::HalfEdge *edge) {
        if (edge->opposite) {
          const auto centroid = edge->opposite->face->centroid();
          const float dot_product = glm::dot(glm::normalize(centroid - position), SquareToRound(offset));
          if (dot_product > 0 && dot_product > maximum) {
            maximum = dot_product;
            argmax = edge->opposite->face;
            target = centroid;
          }
        }
      });
      if (argmax) {
        current_state.player.direction_target = glm::normalize(target - position);
        current_state.player.position_target = target;
      } else if (glm::length(offset) > 0) {
        current_state.player.direction_target = glm::normalize(SquareToRound(offset));
      }
      auto desired_velocity = 25.0f * glm::length(SquareToRound(offset)) * (current_state.player.position_target - position);
      auto force = current_state.player_body->GetMass() * (desired_velocity - current_velocity);
      current_state.player_body->ApplyForceToCenter(b2Vec2(force.x, force.y));
      auto dt = 0.016f;
      if (glm::length(offset) == 0 &&
          joystick.GetButtonPressure(Joystick::PressureButton::kX) == 0) {
        dt *= glm::length(SquareToRound(offset2));
      }
      current_state.world.Step(dt, 8, 3);
//      if (glm::length(offset) > 0) {
//        current_state.player.direction_target = glm::normalize(offset);
//      }
      {
        if (glm::length(offset2) > 0) {
          current_state.player_view_direction_target = glm::normalize(offset2);
        } else {
          current_state.player_view_direction_target = current_state.player.direction_target;
        }
        const float angle = glm::atan(current_state.player_view_direction.y,
                                      current_state.player_view_direction.x);
        float angle_target = glm::atan(current_state.player_view_direction_target.y,
                                       current_state.player_view_direction_target.x);
        while (angle_target - angle > M_PI) {
          angle_target -= 2.0 * M_PI;
        }
        while (angle_target - angle < -M_PI) {
          angle_target += 2.0 * M_PI;
        }
        const float final_angle = glm::mix(angle, angle_target, 0.1f / 0.016f * dt);
        current_state.player_view_direction = glm::vec2(glm::cos(final_angle),
                                                        glm::sin(final_angle));
      }
      current_state.player = UpdateCharacter(current_state.player, dt);
      int index;
      std::string phrases[] = {
        "Someone brushes hurriedly past you.",
        "You pass someone.",
        "One of those thugs shoves past you.",
        "Someone bumps into you.",
        "Someone bumps your arm.",
        "Someone passes by.",
        "A passerby approaches and departs."
      };
      for (auto &character : current_state.non_player_characters) {
        character = UpdateNonPlayerCharacter(character);
        character.character = UpdateCharacter(character.character, dt);
        if (glm::length(character.character.position - current_state.player.position) < 0.1) {
          if (last_approach_times.end() == last_approach_times.find(index)) {
            last_approach_times.insert({index, clock.now()});
            reply_queue.PushMessage(phrases[phrase_index++ % 7]);
          } else if (clock.now() - last_approach_times.at(index) > std::chrono::seconds(2)) {
            reply_queue.PushMessage(phrases[phrase_index++ % 7]);
            last_approach_times.at(index) = clock.now();
          }
        }
        index += 1;
      }
    }
  }

  CharacterInfo Updater::UpdateCharacter(CharacterInfo current_character, float dt) const {
    CharacterInfo next_character = current_character;
    if (next_character.room_target) {
      auto current_face = FindFaceThatContainsPoint(next_character.position);
      if (current_face && next_character.room_target == current_face->room_info) {
        next_character.room_target = nullptr;
      } else if (current_face && next_character.room_target) {
        std::unordered_map<Mesh::Face *, float> distances;
        for (auto &face : mesh.get_faces()) {
          if (next_character.room_target == face->room_info) {
            distances.insert({face.get(), 0.0f});
          } else {
            distances.insert({face.get(), std::numeric_limits<float>::infinity()});
          }
        }
        bool done = false;
        while (!done) {
          done = true;
          for (auto &face : mesh.get_faces()) {
            if (std::numeric_limits<float>::infinity() == distances.at(face.get())) {
              done = false;
            } else {
              continue;
            }
            float minimum = std::numeric_limits<float>::infinity();
            glm::vec2 target;
            Mesh::Face *argmin = nullptr;
            face->ForEachHalfEdge([&] (Mesh::HalfEdge *edge) {
              if (edge->opposite) {
                const float distance = distances.at(edge->opposite->face);
                if (distance < minimum) {
                  minimum = distance;
                  argmin = edge->opposite->face;
                  target = edge->opposite->face->centroid();
                }
              }
            });
            if (argmin) {
              const auto h01 = face->face_edge;
              const auto h12 = h01->next;
              const auto h20 = h12->next;
              CHECK_STATE(h01 == h20->next);
              const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
              distances.at(face.get()) = minimum + glm::length(target - face->centroid());
            }
          }
        }
        float minimum = std::numeric_limits<float>::infinity();
        glm::vec2 target;
        Mesh::Face *argmin = nullptr;
        current_face->ForEachHalfEdge([&] (Mesh::HalfEdge *edge) {
          if (edge->opposite) {
            const float distance = distances.at(edge->opposite->face);
            if (distance < minimum) {
              minimum = distance;
              argmin = edge->opposite->face;
              target = edge->opposite->face->centroid();
            }
          }
        });
        if (argmin) {
          next_character.direction_target = glm::normalize(target - next_character.position);
          next_character.position_target = target;
        }
      }
    }
    const float angle = glm::atan(next_character.direction.y, next_character.direction.x);
    float angle_target = glm::atan(next_character.direction_target.y,
                                   next_character.direction_target.x);
    while (angle_target - angle > M_PI) {
      angle_target -= 2.0 * M_PI;
    }
    while (angle_target - angle < -M_PI) {
      angle_target += 2.0 * M_PI;
    }
    const float final_angle = glm::mix(angle, angle_target, 0.1f / 0.016f * dt);
    next_character.direction = glm::vec2(glm::cos(final_angle), glm::sin(final_angle));
    next_character.position = glm::mix(next_character.position,
                                       next_character.position_target, 0.1f / 0.016f * dt);
    return next_character;
  }

  NonPlayerCharacterInfo Updater::UpdateNonPlayerCharacter(NonPlayerCharacterInfo current_character) const {
    if (!current_character.character.room_target && mesh.get_room_infos().size()) {
      int index = current_character.ai_state.room_target_index % mesh.get_room_infos().size();
      current_character.ai_state.room_target_index += 1;
      current_character.character.room_target = mesh.get_room_infos()[index].get();
    }
    return current_character;
  }

  glm::vec2 Updater::SupremumNormalize(glm::vec2 vector) {
    return vector / std::max(glm::abs(vector.x), glm::abs(vector.y));
  }

  glm::vec2 Updater::SquareToRound(glm::vec2 vector) {
    auto supremum_normalized = SupremumNormalize(vector);
    if (glm::length(supremum_normalized) > 0) {
      vector /= glm::length(supremum_normalized);
    }
    return vector;
  }

  bool Updater::FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const {
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

  Mesh::Face *Updater::FindFaceThatContainsPoint(glm::vec2 point) const {
    for (auto &face : mesh.get_faces()) {
      if (FaceContainsPoint(face.get(), point)) {
        return face.get();
      }
    }
    return nullptr;
  }

}  // namespace textengine
