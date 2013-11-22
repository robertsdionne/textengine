#include <Box2D/Box2D.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <algorithm>
#include <cmath>
#include <fftw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <limits>
#include <mach-o/getsect.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "checks.h"
#include "gamestate.h"
#include "input.h"
#include "keyboard.h"
#include "log.h"
#include "mesh.h"
#include "raycast.h"
#include "synchronizedqueue.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(SynchronizedQueue &command_queue, SynchronizedQueue &reply_queue,
                   Log &playtest_log, Input &input,
                   Mesh &mesh, GameState &initial_state)
  : command_queue(command_queue), reply_queue(reply_queue), playtest_log(playtest_log),
    input(input), mesh(mesh), current_state(initial_state), clock(),
    last_approach_times(), phrase_index(), device(), context() {}

  GameState &Updater::GetCurrentState() {
    return current_state;
  }
  
  ALuint Updater::ReadSoundFile(const std::string &filename) {
    auto data = std::vector<short>();
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, data.data(), data.size() * sizeof(short), 44100);
    return 0;
  }

  void Updater::Setup() {
    shoot[0] = ReadSoundFile("../resource/sound/shoot00.wav");
    
    device = alcOpenDevice(nullptr);
    CHECK_STATE(device);
    context = alcCreateContext(device, nullptr);
    CHECK_STATE(context);
    alcMakeContextCurrent(context);
    CHECK_STATE(!alGetError());
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);
    CHECK_STATE(!alGetError());
    
    const auto kAmount = 44100/2;
    const auto index = index_distribution(generator) % 90000;
    short *amplitudes = reinterpret_cast<short *>(get_etext() - index);
    
    fftw_complex *in, *out;
    fftw_plan p;
    const auto kN = kAmount;
    in = reinterpret_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * kN));
    out = reinterpret_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * kN));
    p = fftw_plan_dft_1d(kN, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    for (auto i = 0; i < kN; ++i) {
      in[i][0] = static_cast<double>(amplitudes[i]) / std::numeric_limits<short>::max();
      in[i][1] = 0.0;
    }
    fftw_execute(p);
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    
    auto data = std::vector<short>();
    data.reserve(kAmount);
    for (auto i = 0; i < kAmount; ++i) {
      short sample = std::numeric_limits<short>::max() * out[i][1] / 100.0;
      data.push_back(sample);
    }
    alBufferData(buffer, AL_FORMAT_MONO16, data.data(), static_cast<ALsizei>(data.size()), 44100);
    alSourcei(source, AL_BUFFER, buffer);
    alSource3f(source, AL_POSITION, 0.0f, 5.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_TRUE);
    alSourcef(source, AL_REFERENCE_DISTANCE, 0.1f);
    alSourcePlay(source);
    alDistanceModel(AL_EXPONENT_DISTANCE);
    CHECK_STATE(!alGetError());

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
    auto offset = input.GetPrimaryAxes();
    auto offset2 = input.GetSecondaryAxes();

    auto dt = 0.016f;
    auto position = glm::vec2(current_state.player_body->GetPosition().x,
                              current_state.player_body->GetPosition().y);
    current_state.world.ClearForces();
    auto velocity = current_state.player_body->GetLinearVelocity();
    const auto force = 10.0f * current_state.player_body->GetMass();
    constexpr auto kMaxVelocity = 0.25f;
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
    if (velocity.Length() > kMaxVelocity) {
      velocity.Normalize();
      velocity *= kMaxVelocity;
      current_state.player_body->SetLinearVelocity(velocity);
    }

    const auto now = std::chrono::high_resolution_clock::now();
    auto remove = [now] (GameState::Shot &shot) {
      return now > shot.death;
    };
    current_state.shots.erase(std::remove_if(current_state.shots.begin(),
                                             current_state.shots.end(), remove),
                              current_state.shots.end());

    if (input.GetTriggerVelocity() > 0.0f) {
      auto start = b2Vec2(position.x, position.y);
      auto error = 0.1f * (distribution(generator) + distribution(generator) - 1.0f);
      auto direction = b2Vec2(glm::cos(angle + error), glm::sin(angle + error));
      auto end = start + direction;
      RayCast raycast;
      current_state.world.RayCast(&raycast, start, end);
      current_state.shots.push_back({
        glm::vec2(start.x, start.y),
        raycast.point,
        now + std::chrono::seconds(1)
      });
      auto n = 0;
      while (glm::abs(direction.x * raycast.normal.x + direction.y * raycast.normal.y) < 0.2f && n < 3) {
        const auto dir = glm::reflect(glm::vec2(direction.x, direction.y), raycast.normal);
        auto error = 0.1f * (distribution(generator) + distribution(generator) - 1.0f);
        const auto st = raycast.point + dir * 1e-5f;
        const auto ang = glm::atan(dir.y, dir.x) + error;
        start = b2Vec2(st.x, st.y);
        direction = b2Vec2(glm::cos(ang + error), glm::sin(ang + error));
        end = start + direction;
        current_state.world.RayCast(&raycast, start, end);
        current_state.shots.push_back({
          glm::vec2(start.x, start.y),
          raycast.point,
          now + std::chrono::seconds(1)
        });
        ++n;
      }
    }

    float orientation[] = {
      glm::cos(angle), glm::sin(angle), 0.0f,
      0.0f, 0.0f, 1.0f
    };
    alListener3f(AL_POSITION, 10.0f * position.x, 10.0f * position.y, 1.5f);
    alListener3f(AL_VELOCITY, 10.0f * velocity.x, 10.0f * velocity.y, 0.0f);
    alListenerfv(AL_ORIENTATION, orientation);
    CHECK_STATE(!alGetError());



    current_state.world.Step(dt, 8, 3);
    current_state.camera_position = glm::mix(current_state.camera_position, position + 0.25f * offset2, 2e-2f / 0.016f * dt);
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
