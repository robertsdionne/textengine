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
#include <stb_vorbis.h>
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
    short *decoded;
    int channels;
    auto length = stb_vorbis_decode_filename(const_cast<char *>(filename.c_str()), &channels, &decoded);
    CHECK_STATE(1 == channels);
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, decoded, length * sizeof(short), 44100);
    return buffer;
  }

  void Updater::BeginContact(b2Contact* contact) {
    if (contact->GetFixtureA()->GetBody() == current_state.player_body) {
      if (contact->GetFixtureB()->GetBody() != current_state.boundary) {
        bool criminal = *reinterpret_cast<bool *>(contact->GetFixtureB()->GetBody()->GetUserData());
        if (criminal) {
          std::cout << "Player hit criminal." << std::endl;
        } else {
          std::cout << "Player hit pedestrian." << std::endl;
        }
      }
    }
    if (contact->GetFixtureB()->GetBody() == current_state.player_body) {
      if (contact->GetFixtureA()->GetBody() != current_state.boundary) {
        bool criminal = *reinterpret_cast<bool *>(contact->GetFixtureA()->GetBody()->GetUserData());
        if (criminal) {
          std::cout << "Player hit criminal." << std::endl;
        } else {
          std::cout << "Player hit pedestrian." << std::endl;
        }
      }
    }
  }

  void Updater::EndContact(b2Contact* contact) {

  }

  void Updater::Setup() {
    shoot[0] = ReadSoundFile("../resource/sound/shoot00.ogg");
    shoot[1] = ReadSoundFile("../resource/sound/shoot01.ogg");
    shoot[2] = ReadSoundFile("../resource/sound/shoot02.ogg");
    shoot[3] = ReadSoundFile("../resource/sound/shoot03.ogg");
    shoot[4] = ReadSoundFile("../resource/sound/shoot04.ogg");
    shoot[5] = ReadSoundFile("../resource/sound/shoot05.ogg");
    shoot[6] = ReadSoundFile("../resource/sound/shoot06.ogg");
    shoot[7] = ReadSoundFile("../resource/sound/shoot07.ogg");
    shoot[8] = ReadSoundFile("../resource/sound/shoot08.ogg");
    shoot[9] = ReadSoundFile("../resource/sound/shoot09.ogg");
    shoot[10] = ReadSoundFile("../resource/sound/shoot10.ogg");
    
    ricochet[0] = ReadSoundFile("../resource/sound/ricochet00.ogg");
    ricochet[1] = ReadSoundFile("../resource/sound/ricochet01.ogg");
    ricochet[2] = ReadSoundFile("../resource/sound/ricochet02.ogg");
    ricochet[3] = ReadSoundFile("../resource/sound/ricochet03.ogg");
    ricochet[4] = ReadSoundFile("../resource/sound/ricochet04.ogg");
    ricochet[5] = ReadSoundFile("../resource/sound/ricochet05.ogg");
    ricochet[6] = ReadSoundFile("../resource/sound/ricochet06.ogg");
    ricochet[7] = ReadSoundFile("../resource/sound/ricochet07.ogg");
    ricochet[8] = ReadSoundFile("../resource/sound/ricochet08.ogg");
    ricochet[9] = ReadSoundFile("../resource/sound/ricochet09.ogg");
    ricochet[10] = ReadSoundFile("../resource/sound/ricochet10.ogg");
    
    device = alcOpenDevice(nullptr);
    CHECK_STATE(device);
    context = alcCreateContext(device, nullptr);
    CHECK_STATE(context);
    alcMakeContextCurrent(context);
    CHECK_STATE(!alGetError());
    alGenSources(1, &shoot_source);
    alGenSources(1, &ricochet_source);
    CHECK_STATE(!alGetError());
    
//    const auto kAmount = 44100/2;
//    const auto index = index_distribution(generator) % 90000;
//    short *amplitudes = reinterpret_cast<short *>(get_etext() - index);
//    
//    fftw_complex *in, *out;
//    fftw_plan p;
//    const auto kN = kAmount;
//    in = reinterpret_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * kN));
//    out = reinterpret_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * kN));
//    p = fftw_plan_dft_1d(kN, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
//    for (auto i = 0; i < kN; ++i) {
//      in[i][0] = static_cast<double>(amplitudes[i]) / std::numeric_limits<short>::max();
//      in[i][1] = 0.0;
//    }
//    fftw_execute(p);
//    fftw_destroy_plan(p);
//    fftw_free(in);
//    fftw_free(out);
    
//    auto data = std::vector<short>();
//    data.reserve(kAmount);
//    for (auto i = 0; i < kAmount; ++i) {
//      short sample = std::numeric_limits<short>::max() * out[i][1] / 100.0;
//      data.push_back(sample);
//    }
//    alBufferData(buffer, AL_FORMAT_MONO16, data.data(), static_cast<ALsizei>(data.size()), 44100);
    shoot_index = ricochet_index = 0;
    alSourcei(shoot_source, AL_LOOPING, AL_FALSE);
    alSourcef(shoot_source, AL_REFERENCE_DISTANCE, 0.25f);
    alSourcei(ricochet_source, AL_LOOPING, AL_FALSE);
    alSourcef(ricochet_source, AL_REFERENCE_DISTANCE, 0.25f);
    alDistanceModel(AL_EXPONENT_DISTANCE);
    CHECK_STATE(!alGetError());

    current_state.world.SetContactListener(this);

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
              distances.at(face.get()) = minimum + 1;//glm::length(target - face->centroid());
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

    if (glm::length(offset) > 0.0 || input.GetTriggerVelocity() > 0.0) {

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

      for (auto rat : current_state.rats) {
        auto current_face = mesh.FindFaceThatContainsPoint(glm::vec2(rat->GetPosition().x, rat->GetPosition().y));
        if (current_face && current_face->room_info && (current_face->room_info->name == "Staircase" ||
                                                        current_face->room_info->name == "Tracks")) {
          Mesh::Face *face = nullptr;
          while (!face || face->room_info) {
            face = mesh.get_faces()[index_distribution(generator) % mesh.get_faces().size()].get();
          }
          auto centroid = face->centroid();
          rat->SetTransform(b2Vec2(centroid.x, centroid.y), rat->GetAngle());
          rat->SetLinearVelocity(b2Vec2(0, 0));
          if (distribution(generator) < 0.02f) {
            *reinterpret_cast<bool *>(rat->GetUserData()) = true;
          } else {
            *reinterpret_cast<bool *>(rat->GetUserData()) = false;
          }
        }
      }

      auto index = 0;
      for (auto rat : current_state.rats) {
        auto current_face = mesh.FindFaceThatContainsPoint(glm::vec2(rat->GetPosition().x, rat->GetPosition().y));
        float minimum = std::numeric_limits<float>::max();
        Mesh::Face *argmin = nullptr;
        if (current_face) {
          current_face->ForEachConnectedFace([&] (Mesh::Face *face) {
            if (distances_to_staircase[face] < minimum) {
              minimum = distances_to_staircase[face];
              argmin = face;
            }
          });
        }
        auto steer = b2Vec2(0, 0);
        if (argmin) {
          auto position = glm::vec2(rat->GetPosition().x, rat->GetPosition().y);
          auto direction = glm::normalize(argmin->centroid() - position);
          auto desired_velocity = 0.5f * b2Vec2(direction.x, direction.y);
          steer = rat->GetMass() * (desired_velocity - rat->GetLinearVelocity());
        }
        rat->ApplyForceToCenter(steer, true);

        auto average_velocity = b2Vec2(0, 0);
        auto average_position = b2Vec2(0, 0);
        auto average_avoid = b2Vec2(0, 0);
        auto count = 0.0f;
        auto avoid_count = 0.0f;

        for (auto other : current_state.rats) {
          if (other != rat && (other->GetPosition() - rat->GetPosition()).Length() < 0.05) {
            average_velocity += other->GetLinearVelocity();
            average_position += other->GetPosition();
            ++count;
          }
          if (other != rat && (other->GetPosition() - rat->GetPosition()).Length() < 0.025) {
            auto avoid = rat->GetPosition() - other->GetPosition();
            avoid.Normalize();
            average_avoid += avoid;
            ++avoid_count;
          }
        }
        auto steer1 = b2Vec2(0, 0);
        auto steer2 = b2Vec2(0, 0);
        auto steer3 = b2Vec2(0, 0);
        auto steer4 = b2Vec2(0, 0);
        auto steer5 = b2Vec2(0, 0);
        if ((current_state.player_body->GetPosition() - rat->GetPosition()).Length() < 0.025) {
          auto avoid = rat->GetPosition() - current_state.player_body->GetPosition();
          avoid.Normalize();
          steer5 = rat->GetMass() * (avoid - velocity);
        }
        auto velocity = rat->GetLinearVelocity();
        if (count > 0) {
          average_velocity = 1.0f / count * average_velocity;
          average_position = 1.0f / count * average_position;
          average_velocity.Normalize();
          average_velocity *= 0.25f;
          steer1 = rat->GetMass() * (average_velocity - velocity);
          auto desired = average_position - rat->GetPosition();
          desired.Normalize();
          desired *= 0.25f;
          auto steer2 = rat->GetMass() * (desired - velocity);
        }
        if (avoid_count > 0) {
          average_avoid = 1.0f / avoid_count * average_avoid;
          average_avoid.Normalize();
          average_avoid *= 0.25f;
          steer3 = rat->GetMass() * (average_avoid - velocity);
        }
        float t = std::chrono::duration_cast<std::chrono::duration<float>>(now.time_since_epoch()).count();
        if (count == 0) {
          auto direction = glm::vec2(glm::simplex(glm::vec3(t, index, 0)), glm::simplex(glm::vec3(t, index, 1)));
          if (glm::length(direction) > 0) {
            direction = 0.5f * glm::normalize(direction);
          }
          steer4 = rat->GetMass() * (b2Vec2(direction.x, direction.y) - velocity);
        } else {
          auto direction = glm::vec2(glm::simplex(glm::vec3(t, index, 0)), glm::simplex(glm::vec3(t, index, 1)));
          if (glm::length(direction) > 0) {
            direction = 0.5f * glm::normalize(direction);
          }
          steer4 = 0.25f * rat->GetMass() * (b2Vec2(direction.x, direction.y) - velocity);
        }
        auto angle = glm::atan((steer + 2.0f * steer1 + steer2 + steer3 + steer4 + 5.0f * steer5).y, (steer + 2.0f * steer1 + steer2 + steer3 + steer4 + 5.0f * steer5).x);
        auto desired_angular_velocity = angle - rat->GetAngle();
        while (angle - rat->GetAngle() > M_PI) {
          angle -= 2.0f * M_PI;
        }
        while (angle - rat->GetAngle() < -M_PI) {
          angle += 2.0f * M_PI;
        }
        rat->SetTransform(rat->GetPosition(), glm::mix(rat->GetAngle(), angle, 0.05f));
        rat->ApplyForceToCenter(2.0f * steer1 + steer2 + steer3 + steer4 + 5.0f * steer5, true);
        ++index;
      }

      current_state.shots.clear();
      //    auto remove = [now] (GameState::Shot &shot) {
      //      return now > shot.death;
      //    };
      //    current_state.shots.erase(std::remove_if(current_state.shots.begin(),
      //                                             current_state.shots.end(), remove),
      //                              current_state.shots.end());

      if (input.GetTriggerVelocity() > 0.0f) {
        current_state.flashlight_on = !current_state.flashlight_on;
      }
      if (current_state.flashlight_on) {
        for (auto i = 0; i < 200; ++i) {

          //      alSourceStop(shoot_source);
          //      alSourcei(shoot_source, AL_BUFFER, shoot[shoot_index++ % 11]);
          //      CHECK_STATE(!alGetError());
          //      alSource3f(shoot_source, AL_POSITION, 10.0f * position.x, 10.0f * position.y , 0.0f);
          //      alSourcePlay(shoot_source);

          auto start = b2Vec2(position.x, position.y);
          auto error = 0.25f * (distribution(generator) + distribution(generator) - 1.0f);
          auto direction = b2Vec2(glm::cos(angle + error), glm::sin(angle + error));
          auto end = start + direction;
          auto intensity = 1.0f;
          RayCast raycast;
          current_state.world.RayCast(&raycast, start, end);
          current_state.shots.push_back({
            glm::vec2(start.x, start.y),
            raycast.point,
            now + std::chrono::seconds(1),
            intensity
          });
          auto n = 0;
          while (glm::abs(direction.x * raycast.normal.x + direction.y * raycast.normal.y) < 1.0f && n < 3) {
            intensity *= 0.5f;
            const auto dir = glm::reflect(glm::vec2(direction.x, direction.y), raycast.normal);
            auto error = 0.25f * (distribution(generator) + distribution(generator) - 1.0f);
            const auto st = raycast.point + dir * 1e-5f;
            const auto ang = glm::atan(dir.y, dir.x) + error;
            start = b2Vec2(st.x, st.y);
            direction = b2Vec2(glm::cos(ang + error), glm::sin(ang + error));
            end = start + direction;
            current_state.world.RayCast(&raycast, start, end);
            current_state.shots.push_back({
              glm::vec2(start.x, start.y),
              raycast.point,
              now + std::chrono::seconds(1),
              intensity
            });
            ++n;
            //          alSourceStop(ricochet_source);
            //          alSourcei(ricochet_source, AL_BUFFER, ricochet[ricochet_index++ % 11]);
            //          CHECK_STATE(!alGetError());
            //          alSource3f(ricochet_source, AL_POSITION, 10.0f * raycast.point.x, 10.0f * raycast.point.y , 0.0f);
            //          alSourcePlay(ricochet_source);
          }
          //        alSourceStop(shoot_source);
          //        alSourcei(shoot_source, AL_BUFFER, shoot[shoot_index++ % 11]);
          //        CHECK_STATE(!alGetError());
          //        alSource3f(shoot_source, AL_POSITION, 10.0f * raycast.point.x, 10.0f * raycast.point.y , 0.0f);
          //        alSourcePlay(shoot_source);
        }
      }
      
      float orientation[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
      };
      alListener3f(AL_POSITION, 10.0f * position.x, 10.0f * position.y, 1.5f);
      alListener3f(AL_VELOCITY, 10.0f * velocity.x, 10.0f * velocity.y, 0.0f);
      alListenerfv(AL_ORIENTATION, orientation);
      CHECK_STATE(!alGetError());
      
      current_state.world.Step(dt, 8, 3);
    }
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
