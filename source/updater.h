#ifndef TEXTENGINE_UPDATER_H_
#define TEXTENGINE_UPDATER_H_

#include <Box2D/Box2D.h>
#include <chrono>
#include <random>
#include <tuple>
#include <unordered_map>

#include "controller.h"
#include "gamestate.h"
#include "scene.h"

namespace textengine {

  class Input;
  class Log;
  class SynchronizedQueue;

  class Updater : public Controller, public b2ContactListener {
  public:
    Updater(SynchronizedQueue &reply_queue, Log &playtest_log,
            Input &input, GameState &initial_state, Scene &scene);

    virtual ~Updater() = default;

	virtual void BeginContact(b2Contact* contact) override;

	virtual void EndContact(b2Contact* contact) override;

    GameState &GetCurrentState();

    virtual void Setup() override;

    virtual void Update() override;

  private:
    void Update(GameState &current_state);

    std::tuple<Area *, Object *, b2Body *> ResolveContact(b2Contact *contact) const;

    std::string ChooseMessage(const MessageMap &messages, const std::string &name);

    enum class Direction {
      kNorth,
      kSouth,
      kEast,
      kWest
    };

  private:
    SynchronizedQueue &reply_queue;
    Log &playtest_log;
    Input &input;
    GameState &current_state;
    int phrase_index;
    std::random_device generator;
    std::uniform_real_distribution<float> distribution;
    std::uniform_int_distribution<> index_distribution;
    Scene &scene;

    Direction last_direction;
    std::chrono::high_resolution_clock::time_point last_direction_time;
    std::unordered_map<Object *, std::chrono::high_resolution_clock::time_point> last_touch_time;
  };

}  // namespace textengine

#endif  // TEXTENGINE_UPDATER_H_
