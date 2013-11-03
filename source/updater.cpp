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
    const glm::vec2 offset = glm::vec2(joystick.GetAxis(Joystick::Axis::kLeftX), -joystick.GetAxis(Joystick::Axis::kLeftY)) * 0.016f;
    current_state.player.position += offset;
    current_state.player.position_target += offset;

    const float angle_offset = -joystick.GetAxis(Joystick::Axis::kRightX);
    const float angle = glm::atan(current_state.player.direction.y, current_state.player.direction.x) + angle_offset;
    float angle_target = glm::atan(current_state.player.direction_target.y,
                                   current_state.player.direction_target.x) + angle_offset;
    while (angle_target - angle > M_PI) {
      angle_target -= 2.0 * M_PI;
    }
    while (angle_target - angle < -M_PI) {
      angle_target += 2.0 * M_PI;
    }
    const float final_angle = glm::mix(angle, angle_target, 0.1f);
    current_state.player.direction = glm::vec2(glm::cos(final_angle), glm::sin(final_angle));
    current_state.player = UpdateCharacter(current_state.player);
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
      character.character = UpdateCharacter(character.character);
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

  CharacterInfo Updater::UpdateCharacter(CharacterInfo current_character) const {
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
            Mesh::HalfEdge *edge = face->face_edge;
            do {
              if (edge->opposite) {
                const auto h01 = edge->opposite->face->face_edge;
                const auto h12 = h01->next;
                const auto h20 = h12->next;
                CHECK_STATE(h01 == h20->next);
                const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
                const auto centroid = (v0 + v1 + v2) / 3.0f;
                const float distance = distances.at(edge->opposite->face);
                if (distance < minimum) {
                  minimum = distance;
                  argmin = edge->opposite->face;
                  target = centroid;
                }
              }
              edge = edge->next;
            } while (edge != face->face_edge);
            if (argmin) {
              const auto h01 = face->face_edge;
              const auto h12 = h01->next;
              const auto h20 = h12->next;
              CHECK_STATE(h01 == h20->next);
              const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
              const auto centroid = (v0 + v1 + v2) / 3.0f;
              distances.at(face.get()) = minimum + glm::length(target - centroid);
            }
          }
        }
        float minimum = std::numeric_limits<float>::infinity();
        glm::vec2 target;
        Mesh::Face *argmin = nullptr;
        Mesh::HalfEdge *edge = current_face->face_edge;
        do {
          if (edge->opposite) {
            const auto h01 = edge->opposite->face->face_edge;
            const auto h12 = h01->next;
            const auto h20 = h12->next;
            CHECK_STATE(h01 == h20->next);
            const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
            const auto centroid = (v0 + v1 + v2) / 3.0f;
            const float distance = distances.at(edge->opposite->face);
            if (distance < minimum) {
              minimum = distance;
              argmin = edge->opposite->face;
              target = centroid;
            }
          }
          edge = edge->next;
        } while (edge != current_face->face_edge);
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
    const float final_angle = glm::mix(angle, angle_target, 0.1f);
    next_character.direction = glm::vec2(glm::cos(final_angle), glm::sin(final_angle));
    next_character.position = glm::mix(next_character.position,
                                       next_character.position_target, 0.1f);
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
