#ifndef TEXTENGINE_UPDATER_H_
#define TEXTENGINE_UPDATER_H_

#include <chrono>
#include <unordered_map>

#include "gamestate.h"
#include "mesh.h"

namespace textengine {

  class CommandParser;
  class Joystick;
  class Log;
  class SynchronizedQueue;

  class Updater {
  public:
    Updater(SynchronizedQueue &command_queue,
            SynchronizedQueue &reply_queue, Log &playtest_log, CommandParser &parser,
            Joystick &joystick, Mesh &mesh, const GameState &initial_state);

    virtual ~Updater() = default;

    GameState &GetCurrentState();

    void Update();

  private:
    void Update(GameState &current_state);

    CharacterInfo UpdateCharacter(CharacterInfo current_character) const;

    NonPlayerCharacterInfo UpdateNonPlayerCharacter(NonPlayerCharacterInfo current_character) const;

    bool FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const;

    Mesh::Face *FindFaceThatContainsPoint(glm::vec2 point) const;

  private:
    SynchronizedQueue &command_queue, &reply_queue;
    Log &playtest_log;
    CommandParser &parser;
    Joystick &joystick;
    Mesh &mesh;
    GameState current_state;
    std::chrono::high_resolution_clock clock;
    std::unordered_map<int, std::chrono::high_resolution_clock::time_point> last_approach_times;
    int phrase_index;
  };

}  // namespace textengine

#endif  // TEXTENGINE_UPDATER_H_
