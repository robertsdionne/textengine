#include <Box2D/Box2D.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <limits>
#include <mach-o/getsect.h>
#include <stb_vorbis.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "checks.h"
#include "gamestate.h"
#include "input.h"
#include "keyboard.h"
#include "log.h"
#include "raycast.h"
#include "scene.h"
#include "synchronizedqueue.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(SynchronizedQueue &command_queue, SynchronizedQueue &reply_queue,
                   Log &playtest_log, Input &input, GameState &initial_state, Scene &scene)
  : command_queue(command_queue), reply_queue(reply_queue), playtest_log(playtest_log),
    input(input), current_state(initial_state), clock(),
    last_approach_times(), phrase_index(), device(), context(), scene(scene) {}

  GameState &Updater::GetCurrentState() {
    return current_state;
  }
  
  ALuint Updater::ReadSoundFile(const std::string &filename) {
    short *decoded;
    int channels;
    auto length = stb_vorbis_decode_filename(const_cast<char *>(filename.c_str()), &channels, &decoded);
    CHECK_STATE(1 == channels);
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, decoded, length * sizeof(short), 44100);
    return buffer;
  }

  void Updater::Setup() {
  }

  Mesh::RoomInfo *Updater::FindRoomInfo(const std::string &room) const {
//    for (auto &room_info : mesh.get_room_infos()) {
//      if (room == room_info->name) {
//        return room_info.get();
//      }
//    }
    return nullptr;
  }

  void Updater::CalculateDistanceTo(const std::string &room, std::unordered_map<Mesh::Face *, float> &distances) {
//    auto target_room = FindRoomInfo(room);
//    if (target_room) {
//      for (auto &face : mesh.get_faces()) {
//        if (target_room == face->room_info) {
//          distances.insert({face.get(), 0.0f});
//        } else {
//          distances.insert({face.get(), std::numeric_limits<float>::infinity()});
//        }
//      }
//      auto updates = true;
////      std::cout << "start " << room << std::endl;
//      while (updates) {
//        updates = false;
//        for (auto &face : mesh.get_faces()) {
//          if (distances.at(face.get()) == std::numeric_limits<float>::infinity()) {
//            float minimum = std::numeric_limits<float>::infinity();
//            glm::vec2 target;
//            Mesh::Face *argmin = nullptr;
//            face->ForEachHalfEdge([&] (Mesh::HalfEdge *edge) {
//              if (edge->opposite && !edge->obstacle) {
//                const float distance = distances.at(edge->opposite->face);
//                if (distance < minimum) {
//                  minimum = distance;
//                  argmin = edge->opposite->face;
//                  target = edge->opposite->face->centroid();
//                }
//              }
//            });
//            if (argmin) {
//              //              std::cout << "updated " << face.get() << " " << minimum << std::endl;
//              const auto h01 = face->face_edge;
//              const auto h12 = h01->next;
//              const auto h20 = h12->next;
//              CHECK_STATE(h01 == h20->next);
//              const auto v0 = h01->start->position, v1 = h12->start->position, v2 = h20->start->position;
//              distances.at(face.get()) = minimum + 1;//glm::length(target - face->centroid());
//              updates = true;
//            }
//          }
//        }
//      }
////      std::cout << "stop" << std::endl;
//    }
  }

  void Updater::Update() {
    Update(current_state);
  }

  glm::vec2 Updater::SpawnPosition(const std::string &room) {
    auto room_info = FindRoomInfo(room);
    std::vector<Mesh::Face *> room_faces;
//    for (auto &face : mesh.get_faces()) {
//      if (room_info == face->room_info) {
//        room_faces.push_back(face.get());
//      }
//    }
    if (room_faces.size()) {
      auto index = index_distribution(generator) % room_faces.size();
      return room_faces[index]->centroid();
    } else {
      return glm::vec2();
    }
  }

  void Updater::Update(GameState &current_state) {
    auto offset = input.GetPrimaryAxes();
    auto offset2 = input.GetSecondaryAxes();

    auto dt = 0.016f;
    auto position = glm::vec2(current_state.player_body->GetPosition().x,
                              current_state.player_body->GetPosition().y);

    if (glm::length(offset) > 0.0 || input.GetTriggerVelocity() > 0.0) {
      current_state.world.ClearForces();
      auto velocity = current_state.player_body->GetLinearVelocity();
      const auto force = 200.0f * current_state.player_body->GetMass();
      if (input.GetTriggerVelocity() > 0) {
        reply_queue.PushMessage("You are running.");
      } else if (input.GetTriggerVelocity() < 0) {
        reply_queue.PushMessage("You are walking.");
      }
      const auto max_velocity = glm::mix(1.38f, 5.81f, input.GetTriggerPressure());
      current_state.player_body->ApplyForceToCenter(force * b2Vec2(offset.x, offset.y), true);
      if (glm::length(offset2) > 0.1f) {
        current_state.target_angle = glm::atan(offset2.y, offset2.x);
      } else if (glm::length(offset) > 0.1f) {
        current_state.target_angle = glm::atan(offset.y, offset.x);
      }
      auto angle = current_state.player_body->GetAngle();
      while (current_state.target_angle - angle > M_PI) {
        current_state.target_angle -= 2.0f * M_PI;
      }
      while (current_state.target_angle - angle < -M_PI) {
        current_state.target_angle += 2.0f * M_PI;
      }
      angle = glm::mix(angle, current_state.target_angle, 0.25f);
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
