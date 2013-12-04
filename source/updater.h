#ifndef TEXTENGINE_UPDATER_H_
#define TEXTENGINE_UPDATER_H_

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <chrono>
#include <random>
#include <unordered_map>

#include "controller.h"
#include "gamestate.h"
#include "mesh.h"

namespace textengine {

  class Input;
  class Log;
  class SynchronizedQueue;

  class Updater : public Controller {
  public:
    Updater(SynchronizedQueue &command_queue,
            SynchronizedQueue &reply_queue, Log &playtest_log,
            Input &input, GameState &initial_state);

    virtual ~Updater() = default;

    GameState &GetCurrentState();

    virtual void Setup() override;

    virtual void Update() override;

  private:
    void Update(GameState &current_state);

    template <typename T> int sgn(T val) {
      return (T(0) < val) - (val < T(0));
    }

    Mesh::RoomInfo *FindRoomInfo(const std::string &room) const;

    glm::vec2 SpawnPosition(const std::string &room);

    glm::vec2 SupremumNormalize(glm::vec2 vector);

    glm::vec2 SquareToRound(glm::vec2 vector);

    void CalculateDistanceTo(const std::string &room, std::unordered_map<Mesh::Face *, float> &distances);
    
    ALuint ReadSoundFile(const std::string &filename);

  private:
    SynchronizedQueue &command_queue, &reply_queue;
    Log &playtest_log;
    Input &input;
    GameState &current_state;
    std::chrono::high_resolution_clock clock;
    std::unordered_map<int, std::chrono::high_resolution_clock::time_point> last_approach_times;
    int phrase_index;
    std::random_device generator;
    std::uniform_real_distribution<float> distribution;
    std::uniform_int_distribution<> index_distribution;
    std::unordered_map<Mesh::Face *, float> distances_to_east_platform_edge,
        distances_to_west_platform_edge, distances_to_staircase;
    ALCdevice *device;
    ALCcontext *context;
    ALuint shoot[11], ricochet[11];
    int shoot_index, ricochet_index;
    ALuint shoot_source, ricochet_source;
  };

}  // namespace textengine

#endif  // TEXTENGINE_UPDATER_H_
