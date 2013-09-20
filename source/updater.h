#ifndef TEXTENGINE_UPDATER_H_
#define TEXTENGINE_UPDATER_H_

#include "gamestate.h"

namespace textengine {

  class CommandParser;
  class CommandQueue;
  class Keyboard;

  class Updater {
  public:
    Updater(CommandQueue &queue, CommandParser &parser, const GameState &initial_state);

    virtual ~Updater() = default;

    GameState GetCurrentState();

    void Update();

  private:
    GameState Update(GameState current_state);

  private:
    CommandQueue &queue;
    CommandParser &parser;
    GameState current_state;
  };

}  // namespace textengine

#endif  // TEXTENGINE_UPDATER_H_
