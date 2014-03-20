#ifndef __textengine__updater__
#define __textengine__updater__

#include <Box2D/Box2D.h>
#include <chrono>
#include <glm/glm.hpp>
#include <random>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "controller.h"
#include "gamestate.h"
#include "scene.h"

namespace textengine {

  class Input;
  class Keyboard;
  class Log;
  class Mouse;
  class SynchronizedQueue;

  class Updater : public Controller, public b2ContactListener {
  public:
    Updater(int width, int height, SynchronizedQueue &reply_queue, SynchronizedQueue &voice_queue,
            Log &playtest_log, Input &input, Mouse &mouse, Keyboard &keyboard,
            GameState &initial_state, Scene &scene);

    virtual ~Updater() = default;

    virtual void BeginContact(b2Contact* contact) override;

    virtual void EndContact(b2Contact* contact) override;

    virtual GameState &GetCurrentState();
    
    glm::vec2 GetCursorPosition() const;
    
    virtual void SetModelViewProjection(glm::mat4 model_view_projection);

    virtual void Setup() override;

    virtual void Update() override;

  private:
    void Update(GameState &current_state);

    std::tuple<Object *, Object *, b2Body *> ResolveContact(b2Contact *contact) const;

    std::string ChooseMessage(const MessageMap &messages, const std::string &name);
    
    bool HasMessage(const MessageMap &messages, const std::string &name);

    bool Inside(const std::unique_ptr<Object> &area) const;

    enum class Direction {
      kNorth,
      kSouth,
      kEast,
      kWest
    };

  private:
    int width, height;
    SynchronizedQueue &reply_queue, &voice_queue;
    Log &playtest_log;
    Input &input;
    Mouse &mouse;
    Keyboard &keyboard;
    GameState &current_state;
    int phrase_index;
    std::random_device generator;
    std::uniform_real_distribution<float> distribution;
    std::uniform_int_distribution<> index_distribution;
    Scene &scene;

    Direction last_direction;
    std::chrono::high_resolution_clock::time_point last_direction_time, last_transmit_time;
    std::unordered_map<Object *, std::chrono::high_resolution_clock::time_point> last_touch_time;
    std::unordered_set<Object *> inside;
    
    glm::mat4 model_view_projection;
  };

}  // namespace textengine

#endif /* defined(__textengine__updater__) */
