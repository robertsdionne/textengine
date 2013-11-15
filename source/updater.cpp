#include <cmath>
#include <glm/glm.hpp>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "checks.h"
#include "commandparser.h"
#include "gamestate.h"
#include "input.h"
#include "keyboard.h"
#include "log.h"
#include "mesh.h"
#include "synchronizedqueue.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(SynchronizedQueue &command_queue, SynchronizedQueue &reply_queue,
                   Log &playtest_log, CommandParser &parser, Input &input,
                   Mesh &mesh, GameState &initial_state)
  : command_queue(command_queue), reply_queue(reply_queue), playtest_log(playtest_log),
    parser(parser), input(input), mesh(mesh), current_state(initial_state), clock(),
    last_approach_times(), phrase_index() {}

  GameState &Updater::GetCurrentState() {
    return current_state;
  }

  void Updater::Setup() {
    mesh.UpdateBvh();
    CalculateDistanceTo("Staircase", distances_to_staircase);
    CalculateDistanceTo("East Platform Edge", distances_to_east_platform_edge);
    CalculateDistanceTo("West Platform Edge", distances_to_west_platform_edge);
  }

  Mesh::RoomInfo *Updater::FindRoomInfo(const std::string &room) const {
    for (auto &room_info : mesh.get_room_infos()) {
      if (room == room_info->name) {
        return room_info.get();
      }
    }
    return nullptr;
  }

  void Updater::CalculateDistanceTo(const std::string &room, std::unordered_map<Mesh::Face *, float> &distances) {
    auto target_room = FindRoomInfo(room);
    if (target_room) {
      for (auto &face : mesh.get_faces()) {
        if (target_room == face->room_info) {
          distances.insert({face.get(), 0.0f});
        } else {
          distances.insert({face.get(), std::numeric_limits<float>::infinity()});
        }
      }
      auto updates = true;
//      std::cout << "start " << room << std::endl;
      while (updates) {
        updates = false;
        for (auto &face : mesh.get_faces()) {
          if (distances.at(face.get()) == std::numeric_limits<float>::infinity()) {
            float minimum = std::numeric_limits<float>::infinity();
            glm::vec2 target;
            Mesh::Face *argmin = nullptr;
            face->ForEachHalfEdge([&] (Mesh::HalfEdge *edge) {
              if (edge->opposite && !edge->obstacle) {
                const float distance = distances.at(edge->opposite->face);
                if (distance < minimum) {
                  minimum = distance;
                  argmin = edge->opposite->face;
                  target = edge->opposite->face->centroid();
                }
              }
            });
            if (argmin) {
              //              std::cout << "updated " << face.get() << " " << minimum << std::endl;
              const auto h01 = face->face_edge;
              const auto h12 = h01->next;
              const auto h20 = h12->next;
              CHECK_STATE(h01 == h20->next);
              const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
              distances.at(face.get()) = minimum + glm::length(target - face->centroid());
              updates = true;
            }
          }
        }
      }
//      std::cout << "stop" << std::endl;
    }
  }

  void Updater::Update() {
    Update(current_state);
  }

  glm::vec2 Updater::SpawnPosition(const std::string &room) {
    auto room_info = FindRoomInfo(room);
    std::vector<Mesh::Face *> room_faces;
    for (auto &face : mesh.get_faces()) {
      if (room_info == face->room_info) {
        room_faces.push_back(face.get());
      }
    }
    if (room_faces.size()) {
      auto index = index_distribution(generator) % room_faces.size();
      return room_faces[index]->centroid();
    } else {
      return glm::vec2();
    }
  }

  void Updater::Update(GameState &current_state) {
    if (command_queue.HasMessage()) {
      const std::string message = command_queue.PopMessage().message;
      playtest_log.LogMessage(message);
      parser.Parse(current_state, message);
    }
    current_state.non_player_characters.erase(std::remove_if(current_state.non_player_characters.begin(), current_state.non_player_characters.end(), [] (NonPlayerCharacterInfo &character) {
      return !character.character.room_target;
    }), current_state.non_player_characters.end());
    auto offset = input.GetPrimaryAxes();
    auto offset2 = input.GetSecondaryAxes();
    auto dt = 0.016f;
    if (glm::length(offset) > 0 || glm::length(offset2) > 0 || input.GetXButton() > 0) {
      if (distribution(generator) < 0.1) {
        std::cout << "spawn staircase" << std::endl;
        current_state.non_player_characters.push_back({
          {SpawnPosition("Staircase"), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), FindRoomInfo("East Platform Edge")}
        });
      }
      if (distribution(generator) < 0.1) {
        std::cout << "spawn staircase" << std::endl;
        current_state.non_player_characters.push_back({
          {SpawnPosition("Staircase"), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), FindRoomInfo("West Platform Edge")}
        });
      }
      if (distribution(generator) < 0.05) {
        std::cout << "spawn train" << std::endl;
        current_state.non_player_characters.push_back({
          {SpawnPosition("East Platform Edge"), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), FindRoomInfo("Staircase")}
        });
      }
      if (distribution(generator) < 0.05) {
        std::cout << "spawn train" << std::endl;
        current_state.non_player_characters.push_back({
          {SpawnPosition("West Platform Edge"), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), FindRoomInfo("Staircase")}
        });
      }
      auto position = current_state.player.position;
      auto current_face = mesh.FindFaceThatContainsPoint(position);
      float maximum = -std::numeric_limits<float>::infinity();
      glm::vec2 target;
      Mesh::Face *argmax = nullptr;
      if (current_face) {
        current_face->ForEachHalfEdge([&] (Mesh::HalfEdge *edge) {
          if (edge->opposite && !edge->obstacle) {
            const auto centroid = edge->opposite->face->centroid();
            const float dot_product = glm::dot(glm::normalize(centroid - position), SquareToRound(offset));
            if (dot_product > 0 && dot_product > maximum) {
              maximum = dot_product;
              argmax = edge->opposite->face;
              target = centroid;
            }
          }
        });
      }
      if (argmax) {
        current_state.player.direction_target = glm::normalize(target - position);
        current_state.player.position_target = target;
      } else if (glm::length(offset) > 0) {
        current_state.player.direction_target = glm::normalize(SquareToRound(offset));
      }
      if (glm::length(offset) == 0 && input.GetXButton() == 0) {
        dt *= glm::length(SquareToRound(offset2));
      }
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
      current_state.player = UpdateCharacter(current_state.player, dt, dt * glm::length(SquareToRound(offset)));
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
        character.character = UpdateCharacter(character.character, dt, dt);
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
    current_state.camera_position = glm::mix(current_state.camera_position, current_state.player.position, 2e-2f / 0.016f * dt);
//    if (joystick.GetButtonVelocity(Joystick::Button::kX) > 0) {
//      mesh.ExtrudeGenerativeEdges(current_state.player.position);
//    }
  }

  CharacterInfo Updater::UpdateCharacter(CharacterInfo current_character, float dt, float dt2) {
    CharacterInfo next_character = current_character;
    if (next_character.room_target) {
      auto current_face = mesh.FindFaceThatContainsPoint(next_character.position);
      if (current_face && next_character.room_target == current_face->room_info) {
        next_character.room_target = nullptr;
      } else if (current_face && next_character.room_target) {
        const std::unordered_map<Mesh::Face *, float> *distances = nullptr;
        if ("Staircase" == next_character.room_target->name) {
          distances = &distances_to_staircase;
        } else if ("East Platform Edge" == next_character.room_target->name) {
          distances = &distances_to_east_platform_edge;
        } else if ("West Platform Edge" == next_character.room_target->name) {
          distances = &distances_to_west_platform_edge;
        }
        if (distances) {
          float minimum = std::numeric_limits<float>::infinity();
          glm::vec2 target;
          Mesh::Face *argmin = nullptr;
          current_face->ForEachHalfEdge([&] (Mesh::HalfEdge *edge) {
            if (edge->opposite && !edge->obstacle) {
              const float distance = distances->at(edge->opposite->face);
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
                                       next_character.position_target, 0.1f / 0.016f * dt2);
    return next_character;
  }

  NonPlayerCharacterInfo Updater::UpdateNonPlayerCharacter(NonPlayerCharacterInfo current_character) {
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

}  // namespace textengine
